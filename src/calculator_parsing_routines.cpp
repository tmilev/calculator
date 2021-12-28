// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"

std::string SyntacticElement::getIntegerStringCrashIfNot(CalculatorParser& owner) {
  if (this->controlIndex != owner.conInteger()) {
    global.fatal << "Request to get rational from a non-rational element. " << global.fatal;
  }
  std::string result = this->data.getValue<std::string>();
  for (unsigned i = 0; i < result.size(); i ++) {
    if (!MathRoutines::isADigit(result[i])) {
      global.fatal << "Integer string non-digit entries. " << global.fatal;
    }
  }
  return result;
}

bool SyntacticElement::isCommandEnclosure() const {
  Calculator* owner = this->data.owner;
  if (this->data.owner == nullptr) {
    return false;
  }
  if (
    this->controlIndex != owner->parser.conExpression() &&
    this->controlIndex != owner->parser.conSequenceStatements()
  ) {
    return false;
  }
  if (this->data.startsWith(owner->opCommandEnclosure())) {
    return true;
  }
  if (this->data.startsWith(owner->opCommandSequence()) && this->data.size() >=2) {
    if (this->data[1].startsWith(owner->opCommandEnclosure())) {
      return true;
    }
  }
  return false;
}

std::string SyntacticElement::toStringSyntaxRole(const CalculatorParser& owner) const {
  if (this->controlIndex < 0) {
    return "Control index not initialized. ";
  }
  return owner.controlSequences[this->controlIndex];
}

std::string SyntacticElement::toStringHumanReadable(
  CalculatorParser& owner, bool includeLispifiedExpressions
) const {
  std::string controlString = this->toStringSyntaxRole(owner);
  bool makeTable = false;
  if (
    this->controlIndex == owner.conExpression() ||
    this->controlIndex == owner.conError() ||
    this->controlIndex == owner.conSequence() ||
    this->controlIndex == owner.conSequenceStatements() ||
    this->controlIndex == owner.conVariable() ||
    this->controlIndex == owner.conInteger() ||
    this->controlIndex == owner.conIntegralSuperScript() ||
    this->controlIndex == owner.conIntegralUnderscore() ||
    this->controlIndex == owner.conIntegralSuperSubScript()
  ) {
    makeTable = true;
  }
  if (!makeTable) {
    return controlString;
  }
  std::stringstream out;
  out << "<table style = 'vertical-align:top;border-spacing:0px 0px'>";
  out << "<tr><td style = 'text-align:center'>"
  << this->data.toString(nullptr) << "</td></tr>";
  out << "<tr><td style = 'color:#AAAAAA'>" << controlString << "</td></tr>";
  if (includeLispifiedExpressions) {
    out << "<tr><td style ='color:#AAAAAA'>" << this->data.toStringFull() << "</td></tr>";
  }
  if (this->errorString != "") {
    out << "<tr><td>" << this->errorString << "</td></tr>";
  }
  out << "</table>";
  return out.str();
}

bool CalculatorParser::replaceIntegerDotIntegerByE() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  std::string afterDecimalPoint = right.getIntegerStringCrashIfNot(*this);
  std::string beforeDecimalPoint = left.getIntegerStringCrashIfNot(*this);
  int powerOfTenToDivideBy = static_cast<int>(afterDecimalPoint.size());
  Rational denominator = 10;
  denominator.raiseToPower(powerOfTenToDivideBy);
  Rational result;
  result.assignString(beforeDecimalPoint);
  result *= denominator;
  result += afterDecimalPoint;
  result /= denominator;
  left.controlIndex = this->conExpression();
  left.data.assignValue(*this->owner, result);
  this->decreaseStackSetCharacterRanges(2);
  return true;
}

void CalculatorParser::reset() {
  this->numberOfEmptyTokensStart = 9;
  this->flagLogSyntaxRules = false;
  this->controlSequences.clear();

  this->syntacticSoup.setSize(0);
  this->syntacticStack.setSize(0);
  this->syntaxErrors = "";
  this->currentSyntacticStack = &this->syntacticStack;
  this->currrentSyntacticSoup = &this->syntacticSoup;
  this->stringsThatSplitIfFollowedByDigit.clear();
}

SyntacticElement CalculatorParser::getSyntacticElementEnd() {
  SyntacticElement result;
  result.controlIndex = this->controlSequences.getIndex(";");
  return result;
}

bool CalculatorParser::decreaseStackSetCharacterRanges(int decrease) {
  if (decrease <= 0) {
    return true;
  }
  if ((*this->currentSyntacticStack).size - decrease <= 0) {
    global.fatal << "Bad stack decrease. " << global.fatal;
  }
  (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size - decrease);
  return true;
}

bool CalculatorParser::parse(
  const std::string& input,
  bool stripCommandSequence,
  Expression& output
) {
  if (!this->parseAndExtractExpressions(
    input, output, this->syntacticSoup, this->syntacticStack, nullptr
  )) {
    return false;
  }
  if (stripCommandSequence) {
    if (output.startsWith(this->owner->opCommandSequence(), 2)) {
      output = output[1];
    }
  }
  return true;
}

void CalculatorParser::initializeControlSequences() {
  // Empty token comes first.
  this->controlSequences.addOnTopNoRepetitionMustBeNew(" ");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("{{}}");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("Variable");
  this->controlSequences.addListOnTop(this->owner->operations.keys); //all operations defined up to this point are also control sequences
  this->controlSequences.addOnTopNoRepetitionMustBeNew("Expression");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("Integer");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("{}");
  this->controlSequences.addOnTopNoRepetitionMustBeNew(",");
  this->controlSequences.addOnTopNoRepetitionMustBeNew(".");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("~");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\choose");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\frac");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\cdot");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("_");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("(");
  this->controlSequences.addOnTopNoRepetitionMustBeNew(")");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("[");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("]");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("{");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("}");
  this->controlSequences.addOnTopNoRepetitionMustBeNew(":");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\varnothing");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("pi");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("infty");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("infinity");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("ln");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\ln");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("log");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("sin");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("cos");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("tan");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("cot");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("csc");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("sec");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("sqrt");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("arcsin");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("arccos");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("arctan");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\int_{*}");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\int^{*}");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\int_{*}^{**}");
  this->controlSequences.addOnTopNoRepetition(
    this->owner->knownOperationsInterpretedAsFunctionsMultiplicatively
  );
  this->controlSequences.addOnTopNoRepetitionMustBeNew("SequenceStatements");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("MakeSequence");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\setminus");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("$");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("MatrixEnd");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\begin");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\left");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\right");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("array");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("pmatrix");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\end");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\\\");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\displaystyle");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("&");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("%");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("NoFrac");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("ShowContext");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("NoLogarithmExponentShortcut");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("LogParsing");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("LogEvaluation");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("HidePolynomialDataStructure");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("NumberColors");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("LogRules");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("LogCache");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("LogFull");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("UseBracketForIntervals");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("DontUsePredefinedWordSplits");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("PlotShowJavascriptOnly");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("PlotDetails");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("UseLnInsteadOfLog");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("UseLnAbsInsteadOfLog");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("CalculatorStatus");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("FullTree");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("HideLHS");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("EndProgram");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("\\text");
}

bool CalculatorParser::parseAndExtractExpressionsDefault(
  const std::string& input,
  Expression& output
) {
  return this->parseAndExtractExpressions(
    input,
    output,
    this->syntacticSoup,
    this->syntacticStack,
    &this->syntaxErrors
  );
}

bool CalculatorParser::parseAndExtractExpressions(
  const std::string& input,
  Expression& output,
  List<SyntacticElement>& outputAllSyntacticElements,
  List<SyntacticElement>& outputSyntacticStack,
  std::string* outputSyntacticErrors
) {
  MacroRegisterFunctionWithName("Calculator::parseAndExtractExpressions");
  this->currentSyntacticStack = &outputSyntacticStack;
  this->currrentSyntacticSoup = &outputAllSyntacticElements;
  this->parseFillDictionary(input);
  bool result = this->extractExpressions(output, outputSyntacticErrors);
  this->currentSyntacticStack = &this->syntacticStack;
  this->currrentSyntacticSoup = &this->syntacticSoup;
  return result;
}

std::string CalculatorParser::toStringSyntacticStackHTMLSimple() {
  MacroRegisterFunctionWithName("Calculator::toStringSyntacticStackHTMLSimple");
  if (this->currentSyntacticStack->size == 0) {
    global.fatal << "Unexpected empty syntactic stack." << global.fatal;
  }
  std::stringstream out;
  bool isBad = ((*this->currentSyntacticStack).size > this->numberOfEmptyTokensStart + 1);
  SyntacticElement& lastSyntacticElt = *(*this->currentSyntacticStack).lastObject();
  if ((*this->currentSyntacticStack).size == this->numberOfEmptyTokensStart + 1) {
    if (lastSyntacticElt.controlIndex != this->conExpression()) {
      isBad = true;
    }
  }
  if (!isBad) {
    out << this->currentSyntacticStack->lastObject()->data.toString();
    return out.str();
  }
  for (int i = this->numberOfEmptyTokensStart; i < (*this->currentSyntacticStack).size; i ++) {
    SyntacticElement& currentElt = (*this->currentSyntacticStack)[i];
    if (currentElt.isCommandEnclosure()) {
      continue;
    }
    if (currentElt.controlIndex != this->conExpression()) {
      out << " <b style='color:red'>" << currentElt.toStringSyntaxRole(*this) << "</b> ";
    } else {
      out << currentElt.data.toString();
    }
  }
  return out.str();
}

void CalculatorParser::initializeStringsThatSplitIfFollowedByDigit() {
  MacroRegisterFunctionWithName("CalculatorParser::initializeStringsThatSplitIfFollowedByDigit");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\cdot");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\circ");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\frac");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\ln");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\log");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\sin");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\cos");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\tan");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\cot");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\sec");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\csc");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\arctan");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\arcsin");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\arccos");
}

SyntacticElement CalculatorParser::getEmptySyntacticElement() {
  SyntacticElement result;
  result.controlIndex = this->controlSequences.getIndex(" ");
  result.data.reset(*this->owner);
  return result;
}

Calculator::EvaluationStatistics::EvaluationStatistics() {
  this->reset();
}

void Calculator::EvaluationStatistics::reset() {
  this->expressionsEvaluated = 0;
  this->maximumCallsBeforeReportGeneration = 5000;
  this->callsSinceReport = 0;
  this->totalSubstitutions = 0;
  this->totalEvaluationLoops = 0;
  this->totalPatternMatchesPerformed = 0;
  this->totalEvaluationLoops = 0;

  this->numberOfListsStart = - 1;
  this->numberListResizesStart = - 1;
  this->numberHashResizesStart = - 1;
  this->numberOfSmallAdditionsStart = - 1;
  this->numberOfSmallMultiplicationsStart = - 1;
  this->numberOfSmallGreatestCommonDivisorsStart = - 1;
  this->numberOfLargeAdditionsStart = - 1;
  this->numberOfLargeMultiplicationsStart = - 1;
  this->numberOfLargeGreatestCommonDivisorsStart = - 1;
  this->millisecondsLastLog = - 1;
  this->startTimeEvaluationMilliseconds = - 1;
  this->startParsing = - 1;
  this->lastStopwatchParsing = - 1;
}

void Calculator::reset() {
  this->statistics.reset();
  this->mode = Calculator::Mode::full;
  this->maximumAlgebraicTransformationsPerExpression = 100;
  this->maximumRecursionDepth = 10000;
  this->recursionDepth = 0;

  this->depthRecursionReached = 0;
  this->flagWriteLatexPlots = false;
  this->flagLogEvaluation = false;
  this->flagUseNumberColors = false;
  this->flagLogRules = false;
  this->flagLogCache = false;
  this->flagLogpatternMatching = false;
  this->flagUseLnAbsInsteadOfLogForIntegrationNotation = false;
  this->flagLogFullTreeCrunching = false;
  this->flagNewContextNeeded = true;
  this->flagDisplayFullExpressionTree = false;
  this->flagHidePolynomialBuiltInTypeIndicator = false;
  this->flagUseFracInRationalLaTeX = true;
  this->flagForkingprocessAllowed = true;
  this->flagUsePredefinedWordSplits = true;
  this->flagPlotNoControls = true;
  this->flagPlotShowJavascriptOnly = false;
  this->flagHasGraphics = false;
  this->flagUseBracketsForIntervals = false;

  this->maximumLatexChars = 2000;
  this->objectContainer.reset();

  //this->logEvaluationSteps.setSize(0);
  this->operations.clear();
  this->builtInTypes.clear();
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.clear();
  this->atomsThatFreezeArguments.clear();
  this->atomsNotInterpretedAsFunctions.clear();
  this->atomsNotAllowingChainRule.clear();
  this->atomsWhoseExponentsAreInterpretedAsFunctions.clear();
  this->atomsThatMustNotBeCached.clear();
  this->arithmeticOperations.clear();
  this->knownFunctionsWithComplexRange.clear();
  this->knownDoubleConstants.clear();
  this->knownDoubleConstantValues.setSize(0);
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.clear();
  this->flagTimeLimitErrorDetected = false;
  this->flagFirstErrorEncountered = false;
  this->flagMaxTransformationsErrorEncountered = false;
  this->flagMaxRecursionErrorEncountered = false;
  this->flagAbortComputationASAP = false;
  this->flagDisplayContext = false;
  this->evaluatedExpressionsStack.clear();
  this->evaluationErrors.setSize(0);
  this->programExpression.reset(*this);
  this->cachedExpressions.clear();
  // The expression container must be cleared second to last.
  this->allChildExpressions.clear();
  // The hashes list below is used in computing the hashes of the list above.
  // It must therefore be cleared last.
  this->allChildExpressionHashes.clear();
  this->parser.reset();
}

void Calculator::initialize(Calculator::Mode desiredMode) {
  MacroRegisterFunctionWithName("Calculator::initialize");
  this->reset();
  this->mode = desiredMode;

  this->operations.setExpectedSize(1000);
  this->namedRules.setExpectedSize(500);
  this->builtInTypes.setExpectedSize(50);

  this->formatVisibleStrings.flagExpressionIsFinal = true;

  // Operation List is the very first operation.
  // It signifies a non-atomic expression.
  // Operation List is signified by the empty string.
  // Operation List has index 0.
  this->addOperationNoRepetitionAllowed("");

  this->addOperationNoRepetitionAllowed("=");
  this->addOperationNoRepetitionAllowed(";");
  this->addOperationNoRepetitionAllowed("if=");
  this->addOperationNoRepetitionAllowed("+");
  this->addOperationNoRepetitionAllowed("-");
  this->addOperationNoRepetitionAllowed("/");
  this->addOperationNoRepetitionAllowed("*");
  this->addOperationNoRepetitionAllowed("!");
  this->addOperationNoRepetitionAllowed("mod");
  this->addOperationNoRepetitionAllowed("and");
  this->addOperationNoRepetitionAllowed("or");
  this->addOperationNoRepetitionAllowed("\\times");
  this->addOperationNoRepetitionAllowed("\\otimes");
  this->addOperationNoRepetitionAllowed("\\oplus");
  this->addOperationNoRepetitionAllowed("\\binom");
  this->addOperationNoRepetitionAllowed("\\sqrt");
  this->addOperationNoRepetitionAllowed("\\in");
  this->addOperationNoRepetitionAllowed("[]");
  this->addOperationNoRepetitionAllowed("=:");
  this->addOperationNoRepetitionAllowed("^");
  this->addOperationNoRepetitionAllowed("\\circ");
  this->addOperationNoRepetitionAllowed("\\geq");
  this->addOperationNoRepetitionAllowed("\\leq");
  this->addOperationNoRepetitionAllowed(">");
  this->addOperationNoRepetitionAllowed("<");
  this->addOperationNoRepetitionAllowed("==");
  this->addOperationNoRepetitionAllowed("===");
  this->addOperationNoRepetitionAllowed("\\cup");
  this->addOperationNoRepetitionAllowed("\\sqcup");
  this->addOperationNoRepetitionAllowed("\\cap");
  this->addOperationNoRepetitionAllowed("Error");
  this->addOperationNoRepetitionAllowed("Sequence");
  this->addOperationNoRepetitionAllowed("Matrix");
  this->addOperationNoRepetitionAllowed("Context");
  this->addOperationNoRepetitionAllowed("|");
  this->addOperationNoRepetitionAllowed("\"");
  this->addOperationNoRepetitionAllowed("PolyVars");
  this->addOperationNoRepetitionAllowed("DiffOpVars");
  this->addOperationNoRepetitionAllowed("\\to");
  this->addOperationNoRepetitionAllowed("if");
  this->addOperationNoRepetitionAllowed("\\lim");
  this->addOperationNoRepetitionAllowed("LogBase");
  this->addOperationNoRepetitionAllowed("\\int");

  this->addOperationBuiltInType("Rational");
  this->addOperationBuiltInType("EltZmodP");
  this->addOperationBuiltInType("Double");
  this->addOperationBuiltInType("AlgebraicNumber");
  this->addOperationBuiltInType("PolynomialRational");
  this->addOperationBuiltInType("PolynomialOverANs");
  this->addOperationBuiltInType("PolynomialModuloInteger");
  this->addOperationBuiltInType("PolynomialModuloPolynomialModuloInteger");
  this->addOperationBuiltInType("RationalFunction");
  this->addOperationBuiltInType("RationalFunctionAlgebraicCoefficients");
  this->addOperationBuiltInType("RationalFunctionModuloInteger");
  this->addOperationBuiltInType("string");
  this->addOperationBuiltInType("JSON");
  this->addOperationBuiltInType("ElementUEoverRF");
  this->addOperationBuiltInType("ElementSemisimpleLieAlgebraAlgebraicCoefficients");
  this->addOperationBuiltInType(Calculator::Atoms::elementTensorsGeneralizedVermas);
  this->addOperationBuiltInType("CharSSAlgMod");
  this->addOperationBuiltInType("SemisimpleLieAlg");
  this->addOperationBuiltInType("LittelmannPath");
  this->addOperationBuiltInType("LRO");
  this->addOperationBuiltInType("MatrixRational");
  this->addOperationBuiltInType("MatrixDouble");
  this->addOperationBuiltInType("MatrixAlgebraic");
  this->addOperationBuiltInType("MatrixTensorRational");
  this->addOperationBuiltInType("MatrixRF");
  this->addOperationBuiltInType("MatrixPolynomialRational");
  this->addOperationBuiltInType("CalculusPlot");
  this->addOperationBuiltInType("SemisimpleSubalgebras");
  this->addOperationBuiltInType("CandidateSSsubalgebra");
  this->addOperationBuiltInType("WeylGroup");
  this->addOperationBuiltInType("ElementWeylGroup");
  this->addOperationBuiltInType("HyperoctahedralGroupRepresentation");
  this->addOperationBuiltInType("ElementHyperoctahedral");
  this->addOperationBuiltInType("WeylGroupRep");
  this->addOperationBuiltInType("WeylGroupVirtualRep");
  this->addOperationBuiltInType("ElementWeylAlgebra");
  this->addOperationBuiltInType("weightLieAlg");
  this->addOperationBuiltInType("weightLieAlgPoly");
  this->addOperationBuiltInType("ellipticCurveElementsRational");
  this->addOperationBuiltInType("ellipticCurveElementsZmodP");
  this->addOperationBuiltInType("userInputTextBox");

  this->initializeOperationsInterpretedAsFunctionsMultiplicatively();

  this->parser.initializeControlSequences();


  this->initializePredefinedStandardOperationsWithoutHandler();
  this->initializeBuiltInsFreezeArguments();
  this->initializeFunctionsStandard();
  if (this->mode == Calculator::Mode::full) {
    this->initializeFunctionsCryptoAndEncoding();
    this->initializeFunctionsScientificBasic();
    this->initializeFunctionsSemisimpleLieAlgebras();
    this->initializeAdminFunctions();
    this->initializeFunctionsExtra();
  }
  this->initializeAtomsThatAllowCommutingOfArguments();
  this->initializeAtomsThatFreezeArguments();
  this->initializeAtomsNotGoodForChainRule();
  this->initializeBuiltInAtomsNotInterpretedAsFunctions();
  this->initializeBuiltInAtomsWhosePowersAreInterpretedAsFunctions();
  this->initializeOperationsThatAreKnownFunctions();
  this->initializeAtomsNonCacheable();
  this->initializeArithmeticOperations();
  this->parser.initializePredefinedWordSplits();
  this->parser.initializeStringsThatSplitIfFollowedByDigit();

  this->initializeToStringHandlers();

  this->ruleStack.reset(*this, 100);
  this->ruleStack.addChildAtomOnTop(this->opCommandSequence());
  this->numberOfPredefinedAtoms = this->operations.size(); //<-operations added up to this point are called ``operations''
  this->checkConsistencyAfterInitialization();
}

bool Calculator::checkPredefinedFunctionNameRepetitions() {
  MacroRegisterFunctionWithName("Calculator::checkPredefinedFunctionNameRepetitions");
  HashedList<std::string, MathRoutines::hashString> ruleIds;
  for (int i = 0; i < this->operations.size(); i ++) {
    MemorySaving<Calculator::OperationHandlers>& current = this->operations.values[i];
    if (current.isZeroPointer()) {
      continue;
    }
    List<Function>* currentHandlers = &current.getElement().handlers;
    for (int j = 0; j < 2; j ++) {
      for (int k = 0; k < currentHandlers->size; k ++) {
        std::string& currentName = (*currentHandlers)[k].calculatorIdentifier;
        if (currentName == "") {
          continue;
        }
        if (ruleIds.contains(currentName)) {
          global.fatal << "Calculator identifier: " << currentName << " is not unique. ";
        }
        ruleIds.addOnTopNoRepetitionMustBeNew(currentName);
      }
      currentHandlers = &current.getElement().compositeHandlers;
    }
  }
  return true;
}

bool CalculatorParser::replaceOXEXEXEXByE() {
  SyntacticElement& opElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 8];
  SyntacticElement& leftE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 6];
  SyntacticElement& middleE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& rightE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this->owner, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.data);
  newExpr.addChildOnTop(middleE.data);
  newExpr.addChildOnTop(rightE.data);
  opElt.data = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(7);
}

bool CalculatorParser::replaceSqrtEXByEX() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& argument = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(this->owner->opSqrt());
  newExpr.addChildOnTop(this->owner->expressionTwo());
  newExpr.addChildOnTop(argument.data);
  left.data = newExpr;
  left.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(1);
}

bool CalculatorParser::replaceSqrtEXXByEXX() {
  int lastSize = (*this->currentSyntacticStack).size;
  SyntacticElement& left = (*this->currentSyntacticStack)[lastSize - 4];
  SyntacticElement& argument = (*this->currentSyntacticStack)[lastSize - 3];
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(this->owner->opSqrt());
  newExpr.addChildOnTop(this->owner->expressionTwo());
  newExpr.addChildOnTop(argument.data);
  left.data = newExpr;
  left.controlIndex = this->conExpression();
  (*this->currentSyntacticStack)[lastSize - 3] = (*this->currentSyntacticStack)[lastSize - 2];
  (*this->currentSyntacticStack)[lastSize - 2] = (*this->currentSyntacticStack)[lastSize - 1];
  (*this->currentSyntacticStack).setSize(lastSize - 1);
  return true;
}

bool CalculatorParser::replaceSqrtXEXByEX() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& argument = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(this->owner->opSqrt());
  newExpr.addChildOnTop(this->owner->expressionTwo());
  newExpr.addChildOnTop(argument.data);
  left.data = newExpr;
  left.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(2);
}

bool CalculatorParser::replaceOXEXEByE() {
  SyntacticElement& opElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  SyntacticElement& leftE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& rightE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this->owner, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.data);
  newExpr.addChildOnTop(rightE.data);
  opElt.data = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(4);
}

bool CalculatorParser::replaceOXEXEXByEX() {
  SyntacticElement& opElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 6];
  SyntacticElement& leftE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& rightE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this->owner, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.data);
  newExpr.addChildOnTop(rightE.data);
  opElt.data = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(4);
}

bool CalculatorParser::replaceOXEXEXXByEXX() {
  SyntacticElement& opElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 7];
  SyntacticElement& leftE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  SyntacticElement& rightE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  Expression newExpr;
  newExpr.reset(*this->owner, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.data);
  newExpr.addChildOnTop(rightE.data);
  opElt.data = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->decreaseStackExceptLastTwo(4);
}

void Calculator::logPublicError(const std::string& error) {
  if (this->errorsPublic.str() != "") {
    this->errorsPublic << error;
  }
}

bool CalculatorParser::decreaseStack(int decrease) {
  this->syntacticStack.setSize(this->syntacticStack.size - decrease);
  return true;
}

bool CalculatorParser::decreaseStackExceptLast(int decrease) {
  if (decrease <= 0) {
    return true;
  }
  if ((*this->currentSyntacticStack).size - decrease <= 0) {
    global.fatal << "Bad stack decrease amount. " << global.fatal;
  }
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - decrease - 1] =
  *this->currentSyntacticStack->lastObject();
  (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size - decrease);
  return true;
}

bool CalculatorParser::decreaseStackExceptLastTwo(int decrease) {
  if (decrease <= 0) {
    return true;
  }
  if ((*this->currentSyntacticStack).size - decrease <= 0) {
    global.fatal << "Bad stack decrease amount. " << global.fatal;
  }
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size-decrease - 2] =
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size-decrease - 1] =
  *this->currentSyntacticStack->lastObject();
  (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size - decrease);
  return true;
}

bool CalculatorParser::replaceOXEEXByEX() {
  SyntacticElement& opElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  SyntacticElement& leftE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& rightE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr(*this->owner);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.data);
  newExpr.addChildOnTop(rightE.data);
  opElt.data = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(3);
}

bool CalculatorParser::replaceOXXEXEXEXByE() {
  SyntacticElement& opElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 9];
  SyntacticElement& leftE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 6];
  SyntacticElement& middleE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& rightE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this->owner, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.data);
  newExpr.addChildOnTop(middleE.data);
  newExpr.addChildOnTop(rightE.data);
  opElt.data = newExpr;
  opElt.controlIndex = this->conExpression();
  this->decreaseStackSetCharacterRanges(8);
  return true;
}

bool CalculatorParser::replaceOXEXByEX() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this->owner, 2);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(left.controlIndex));
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  left.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(2);
}

bool CalculatorParser::canBeRegardedAsDifferentialForm(const std::string& input) {
  return StringRoutines::stringBeginsWith(input, "d") && input.size() > 1;
}

bool CalculatorParser::canBeRegardedAsDifferentialForm(const SyntacticElement& input) {
  std::string role = this->controlSequences[input.controlIndex];
  std::string whichAtom;
  if (role == "Variable") {
    whichAtom = this->owner->operations.keys[input.data.data];
    return this->canBeRegardedAsDifferentialForm(whichAtom);
  }
  if (role != "Expression") {
    return false;
  }
  if (!input.data.isAtomUserDefined(&whichAtom)) {
    return false;
  }
  return this->canBeRegardedAsDifferentialForm(whichAtom);
}

bool CalculatorParser::replaceOXXByEXX() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Calculator::replaceOXXByEXX]";
  }
  left.data.makeAtom(*this->owner, this->getOperationIndexFromControlIndex(left.controlIndex));
  left.controlIndex = this->conExpression();
  return true;
}

bool CalculatorParser::replaceOXEByE() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this->owner, 2);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(left.controlIndex));
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  left.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(2);
}

bool CalculatorParser::replaceOEByE() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this->owner, 2);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(left.controlIndex));
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  left.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(1);
}

bool CalculatorParser::replaceOEXByEX() {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceOEXByEX]";
  }
  Expression newExpr;
  newExpr.reset(*this->owner, 2);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.addChildOnTop(right.data);
  middle.data = newExpr;
  middle.controlIndex = this->conExpression();
  this->decreaseStackExceptLast(1);
  return true;
}

bool CalculatorParser::replaceXXXByCon(int controlIndex) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3].controlIndex = controlIndex;
  this->decreaseStackSetCharacterRanges(2);
  return true;
}

bool CalculatorParser::replaceXXXByConCon(int controlIndex1, int controlIndex2) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2].controlIndex = controlIndex1;
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3].controlIndex = controlIndex2;
  this->decreaseStackSetCharacterRanges(1);
  return true;
}

bool CalculatorParser::replaceXXXXXByCon(int controlIndex) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5].controlIndex = controlIndex;
  this->decreaseStackSetCharacterRanges(4);
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceXXXXXByCon]";
  }
  return true;
}

bool CalculatorParser::replaceXXXXXByConCon(int controlIndex1, int controlIndex2) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4].controlIndex = controlIndex2;
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5].controlIndex = controlIndex1;
  this->decreaseStackSetCharacterRanges(3);
  return true;
}

bool CalculatorParser::replaceXXXXByConCon(int controlIndex1, int controlIndex2) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3].controlIndex = controlIndex2;
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4].controlIndex = controlIndex1;
  this->decreaseStackSetCharacterRanges(2);
  return true;
}

bool CalculatorParser::replaceXXXXByCon(int controlIndex1) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4].controlIndex = controlIndex1;
  this->decreaseStackSetCharacterRanges(3);
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceXXXXByCon]";
  }
  return true;
}

bool CalculatorParser::replaceXByCon(int controlIndex) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1].controlIndex = controlIndex;
  return true;
}

bool CalculatorParser::replaceXByO(int operation) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1].controlIndex = this->conExpression();
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1].data.makeAtom(*this->owner, operation);
  return true;
}

bool CalculatorParser::replaceXByConCon(int controlIndex1, int controlIndex2) {
  (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size + 1);
  (*this->currentSyntacticStack).lastObject()->data.reset(*this->owner);
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2].controlIndex = controlIndex1;
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1].controlIndex = controlIndex2;
  return true;
}

bool CalculatorParser::replaceOEXByE() {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceOEXByE]";
  }
  Expression newExpr;
  newExpr.reset(*this->owner, 2);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.addChildOnTop(right.data);
  middle.data = newExpr;
  middle.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(2);
}

bool CalculatorParser::replaceXXByEmptyString() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  left.data.assignValue(*this->owner, std::string(""));
  left.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(1);
}

bool CalculatorParser::replaceEOByE() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this->owner, 2);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(right.controlIndex));
  newExpr.addChildOnTop(left.data);
  left.data = newExpr;
  left.controlIndex = this->conExpression();
  this->decreaseStackSetCharacterRanges(1);
  return true;
}

bool CalculatorParser::isRightSeparator(unsigned char c) {
  switch(c) {
    case ' ':
    case 160: //&nbsp character
    case '\r':
    case '\n':
    case '\t':
    case '>':
    case '<':
    case ':':
    case ',':
    case ';':
    case '+':
    case '*':
    case '^':
    case '=':
    case '_':
    case '%':
    case '/':
    case '-':
    case '`':
    case '[':
    case '&':
    case '\\':
    case '}':
    case '{':
    case '~':
    case '$':
    case ']':
    case '(':
    case ')':
    case '.':
    case '!':
    case '|':
    case '\"':
      return true;
    default:
      return false;
  }
}

bool CalculatorParser::isLeftSeparator(unsigned char c) {
  switch(c) {
    case ' ':
    case 160: //&nbsp character
    case '\r':
    case '\n':
    case '\t':
    case '>':
    case '<':
    case ':':
    case ',':
    case ';':
    case '+':
    case '*':
    case '^':
    case '&':
    case '=':
    case '_':
    case '/':
    case '-':
    case '`':
    case '[':
    case '%':
    case '}':
    case '{':
    case '~':
    case '$':
    case ']':
    case '(':
    case ')':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '.':
    case '!':
    case '|':
    case '\"':
      return true;
    default:
      return false;
  }
}

bool CalculatorParser::setStackValue(const Expression& newExpression, const std::string& newRole, int stackOffset) {
  this->syntacticStack[this->syntacticStack.size + stackOffset].data = newExpression;
  this->syntacticStack[this->syntacticStack.size + stackOffset].controlIndex = this->controlSequences.getIndexNoFail(newRole);
  return true;
}

bool CalculatorParser::isInterpretedAsEmptySpace(const std::string& input) {
  if (input.size() != 1) {
    return false;
  }
  return this->isInterpretedAsEmptySpace(static_cast<unsigned char>(input[0]));
}

bool CalculatorParser::isInterpretedAsEmptySpace(unsigned char input) {
  switch (input) {
   
  case '\n': //numerical value: 10
  case '\r': //numerical value: 13
  case '\t':
  case 160: //&nbsp
    return true;
  default:
    return false;
  }
  // return false;
}

void CalculatorParser::initializePredefinedWordSplits() {
  MacroRegisterFunctionWithName("Calculator::initializePredefinedWordSplits");
  List<std::string> splitVariables;
  splitVariables.addOnTop("x");
  splitVariables.addOnTop("y");
  this->predefinedWordSplits.setKeyValue("xy", List<std::string>({"x", "y"}));
  this->predefinedWordSplits.setKeyValue("xdx", List<std::string>({"x", "dx"}));
  this->predefinedWordSplits.setKeyValue("yx", List<std::string>({"y", "x"}));
  this->addTrigonometricSplit("int", splitVariables);
  this->addTrigonometricSplit("sin", splitVariables);
  this->addTrigonometricSplit("cos", splitVariables);
  this->addTrigonometricSplit("tan", splitVariables);
  this->addTrigonometricSplit("cot", splitVariables);
  this->addTrigonometricSplit("sec", splitVariables);
  this->addTrigonometricSplit("csc", splitVariables);
}

void CalculatorParser::parseFillDictionary(const std::string& input) {
  MacroRegisterFunctionWithName("Calculator::parseFillDictionary");
  this->parseFillDictionary(input, *this->currrentSyntacticSoup);
  SyntacticElement currentElement;
  currentElement.data.reset(*this->owner);
  currentElement.controlIndex = this->conEndProgram();
  (*this->currrentSyntacticSoup).addOnTop(currentElement);
}

bool CalculatorParser::shouldSplitOutsideQuotes(const std::string& left, char right) {
  if (left.size() == 0) {
    return false;
  }
  char leftLastChar = left[left.size() - 1];
  bool rightIsDigit = MathRoutines::isADigit(right);
  bool leftIsDigit = MathRoutines::isADigit(leftLastChar);
  if (leftIsDigit && rightIsDigit) {
    return false;
  }
  if  (
    this->isLeftSeparator(static_cast<unsigned char>(left[0])) ||
    this->isRightSeparator(static_cast<unsigned char>(right)) ||
    left == " "
  ) {
    return true;
  }
  if (rightIsDigit) {
    if (leftLastChar == '\\' || this->stringsThatSplitIfFollowedByDigit.contains(left)) {
      return true;
    }
  }
  return false;
}

void CalculatorParser::parseConsumeQuote(
  const std::string& input,
  unsigned int& indexOfLast,
  List<SyntacticElement>& output
) {
  SyntacticElement quoteStart, quoteEnd, content;
  quoteStart.controlIndex = this->conQuote();
  quoteEnd = quoteStart;
  content.controlIndex = this->conExpression();
  output.addOnTop(quoteStart);
  bool previousIsUnescapedBackslash = false;
  std::string consumed;
  for (indexOfLast ++; indexOfLast < input.size(); indexOfLast ++) {
    char current = input[indexOfLast];
    if (current == '"' && !previousIsUnescapedBackslash) {
      content.data.makeAtom(*this->owner, consumed);
      output.addOnTop(content);
      output.addOnTop(quoteEnd);
      return;
    }
    if (current == '\\') {
      previousIsUnescapedBackslash = !previousIsUnescapedBackslash;
    } else {
      previousIsUnescapedBackslash = false;
    }
    consumed.push_back(current);
  }
  content.data.makeAtom(*this->owner, consumed);
  output.addOnTop(content);
}

bool CalculatorParser::parseNoEmbeddingInCommand(const std::string& input, Expression& output) {
  return this->parse(input, true, output);
}

bool CalculatorParser::parseEmbedInCommandSequence(const std::string& input, Expression& output) {
  return this->parse(input, true, output);
}

void CalculatorParser::parseFillDictionary(
  const std::string& input, List<SyntacticElement>& output
) {
  MacroRegisterFunctionWithName("Calculator::parseFillDictionary");
  std::string current;
  output.reserve(static_cast<signed>(input.size()));
  output.setSize(0);
  char lookAheadChar;
  SyntacticElement currentElement;
  for (unsigned i = 0; i < input.size(); i ++) {
    char currentCharacter = input[i];
    if (currentCharacter == '"') {
      this->parseConsumeQuote(input, i, output);
      continue;
    }
    current.push_back(currentCharacter);
    if (i + 1 < input.size()) {
      lookAheadChar = input[i + 1];
    } else {
      lookAheadChar = ' ';
    }
    if (this->isInterpretedAsEmptySpace(current)) {
      current = " ";
    }
    if (!this->shouldSplitOutsideQuotes(current, lookAheadChar)) {
      continue;
    }
    bool mustInterpretAsVariable = false;
    if (this->controlSequences.contains(current) && !mustInterpretAsVariable) {
      currentElement.controlIndex = this->controlSequences.getIndex(current);
      currentElement.data.reset(*this->owner);
      output.addOnTop(currentElement);
    } else if (MathRoutines::hasDecimalDigitsOnly(current) && !mustInterpretAsVariable) {
      currentElement.data.assignValue(*this->owner, current);
      currentElement.controlIndex = this->conInteger();
      output.addOnTop(currentElement);
    } else {
      currentElement.controlIndex = this->controlSequences.getIndex("Variable");
      currentElement.data.makeAtom(*this->owner, this->owner->addOperationNoRepetitionOrReturnIndexFirst(current));
      output.addOnTop(currentElement);
    }
    current = "";
  }
}

int CalculatorParser::getOperationIndexFromControlIndex(int controlIndex) {
  return this->owner->operations.getIndex(this->controlSequences[controlIndex]);
}

int CalculatorParser::getExpressionIndex() {
  return this->controlSequences.getIndex("Expression");
}

bool CalculatorParser::replaceXXbyE() {
  this->replaceXXbyEX();
  this->decreaseStackSetCharacterRanges(1);
  return true;
}

bool CalculatorParser::replaceXXXbyE() {
  this->replaceXXbyEX();
  return this->replaceXEXByE();
}

bool CalculatorParser::replaceOXdotsXbyEXdotsX(int numberOfXs) {
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1 - numberOfXs];
  element.data.makeAtom(*this->owner, this->getOperationIndexFromControlIndex(element.controlIndex));
  if (this->flagLogSyntaxRules) {
    std::stringstream out;
    out << "[Rule: Calculator::replaceOXdotsXbyEXdotsX: " << numberOfXs << "]";
    this->lastRuleName = out.str();
  }
  element.controlIndex = this->conExpression();
  return true;
}

bool CalculatorParser::replaceOXbyEX() {
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  element.data.makeAtom(*this->owner, this->getOperationIndexFromControlIndex(element.controlIndex));
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceOXbyEX]";
  }
  return this->replaceXXbyEX();
}

bool CalculatorParser::replaceObyE() {
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  element.data.makeAtom(*this->owner, this->getOperationIndexFromControlIndex(element.controlIndex));
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Calculator::replaceObyE]";
  }
  element.controlIndex = this->conExpression();
  return true;
}

bool CalculatorParser::replaceXXbyEX() {
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  element.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Calculator::replaceXXbyEX]";
  }
  return true;
}

bool CalculatorParser::replaceEXXSequenceXBy_Expression_with_E_instead_of_sequence() {
  SyntacticElement& sequenceElement = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& functionExpressionlt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  Expression newExpr;
  newExpr.reset(*this->owner);
  newExpr.setExpectedSize(sequenceElement.data.size());
  newExpr.addChildOnTop(functionExpressionlt.data);
  if (sequenceElement.data.isAtom()) {
    newExpr.addChildOnTop(sequenceElement.data);
  } else {
    for (int i = 1; i < sequenceElement.data.size(); i ++) {
      newExpr.addChildOnTop(sequenceElement.data[i]);
    }
  }
  functionExpressionlt.data = newExpr;
  return this->decreaseStackSetCharacterRanges(4);
}

bool CalculatorParser::replaceXEXByE() {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3] =
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3].controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Calculator::replaceXEXByE]";
  }
  return this->decreaseStackSetCharacterRanges(2);
}

bool CalculatorParser::replaceXEXYByEY() {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4] =
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4].controlIndex = this->conExpression();
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3] =
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Calculator::replaceXEXYByEY]";
  }
  return this->decreaseStackSetCharacterRanges(2);
}

bool CalculatorParser::replaceXEXByEContainingOE(int inputOpIndex) {
  SyntacticElement& outputElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& inputElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Calculator::replaceXEXByEContainingOE]";
  }
  outputElt.data.reset(*this->owner, 2);
  outputElt.data.addChildAtomOnTop(inputOpIndex);
  outputElt.data.addChildOnTop(inputElt.data);
  outputElt.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(2);
}

bool CalculatorParser::replaceXXByEEmptySequence() {
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  element.data.reset(*this->owner, 1);
  element.data.addChildAtomOnTop(this->owner->opSequence());
  element.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(1);
}

bool CalculatorParser::isBoundVariableInContext(int inputOperation) {
  return this->boundVariablesInContext.contains(inputOperation);
}

bool CalculatorParser::isNonBoundVariableInContext(int inputOperation) {
  return this->nonBoundVariablesInContext.contains(inputOperation);
}

bool CalculatorParser::replaceXXVXdotsXbyE_BOUND_XdotsX(int numberOfXs) {
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - numberOfXs - 1];
  int boundVariable = element.data.data;
  if (this->isNonBoundVariableInContext(boundVariable)) {
    std::stringstream out;
    out << "Syntax error. In the same syntactic scope, the string "
    << this->owner->operations.keys[boundVariable]
    << " is first used to denote a non-bound variable "
    << "but later to denote a bound variable. This is not allowed. ";
    element.errorString = out.str();
    element.controlIndex = this->conError();
    this->decreaseStackSetCharacterRanges(numberOfXs);
    this->replaceXXYByY();
    return true;
  }
  if (!this->isBoundVariableInContext(boundVariable)) {
    this->boundVariablesInContext.addOnTopNoRepetition(boundVariable);
  }
  element.data.reset(*this->owner, 2);
  element.data.addChildAtomOnTop(this->owner->opBind());
  element.data.addChildAtomOnTop(boundVariable);
  element.controlIndex = this->conExpression();
  this->decreaseStackSetCharacterRanges(numberOfXs);
  this->replaceXXYByY();
  return true;
}

bool CalculatorParser::replaceVXdotsXbyE_NONBOUND_XdotsX(int numberOfXs) {
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1 - numberOfXs];
  int boundVariable = element.data.data;
  if (this->isBoundVariableInContext(boundVariable)) {
    element.data.reset(*this->owner, 2);
    element.data.addChildAtomOnTop(this->owner->opBind());
    element.data.addChildAtomOnTop(boundVariable);
  } else {
    element.data.makeAtom(*this->owner, boundVariable);
    if (!this->isNonBoundVariableInContext(boundVariable)) {
      this->nonBoundVariablesInContext.addOnTop(boundVariable);
    }
  }
  element.controlIndex = this->conExpression();
  return true;
}

bool CalculatorParser::replaceOOEEXbyEXpowerLike() {
  SyntacticElement& outerO = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& innerO = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  SyntacticElement& innerArg = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& outerArg = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  if (this->flagLogSyntaxRules) {
    this->parsingLog = "[Calculator::replaceOOEEXbyEXpowerLike]";
  }
  Expression newInnerE(*this->owner);
  Expression newFinalE(*this->owner);
  newInnerE.addChildAtomOnTop(this->getOperationIndexFromControlIndex(innerO.controlIndex));
  newInnerE.addChildOnTop(innerArg.data);
  newFinalE.addChildAtomOnTop(this->getOperationIndexFromControlIndex(outerO.controlIndex));
  newFinalE.addChildOnTop(newInnerE);
  newFinalE.addChildOnTop(outerArg.data);

  innerO.data = newFinalE;
  innerO.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(3);
}

bool CalculatorParser::replaceCXByE() {
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  element.controlIndex = this->conExpression();
  return this->popTopSyntacticStack();
}

bool CalculatorParser::replaceCXByEX() {
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  element.controlIndex = this->conExpression();
  return true;
}

bool CalculatorParser::replaceXdotsXByMatrixStart(int numberOfXs) {
  SyntacticElement& currentElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - numberOfXs];
  currentElt.dataList.setExpectedSize(10);
  currentElt.dataList.setSize(1);
  currentElt.dataList.lastObject()->makeSequence(*this->owner);
  currentElt.controlIndex = this->conMatrixStart();
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Calculator::replaceXdotsXByMatrixStart]";
  }
  return this->decreaseStackSetCharacterRanges(numberOfXs - 1);
}

bool CalculatorParser::replaceMatrixXByE() {
  SyntacticElement& matrixElement = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Matrix<Expression> matrix;
  int numberOfColumns = 0;
  for (int i = 0; i < matrixElement.dataList.size; i ++) {
    numberOfColumns = MathRoutines::maximum(matrixElement.dataList[i].size() - 1, numberOfColumns);
  }
  if (matrixElement.dataList.size > 1) {
    if (matrixElement.dataList.lastObject()->size() - 1 == 0) {
      // We have an empty last row.
      // This is obtained for example when parsing the following.
      // \begin{array}{cc} 1&1\\ 2&3\\ \end{array}
      // where the last \\ creates an empty last row (in our set of parsing rules).
      // We trim that last row.
      matrixElement.dataList.setSize(matrixElement.dataList.size - 1);
    }
  }
  int numberOfRows = matrixElement.dataList.size;
  if (numberOfColumns > 0 && numberOfRows > 0) {
    matrix.initialize(numberOfRows, numberOfColumns);
    for (int i = 0; i < numberOfRows; i ++) {
      for (int j = 0; j < numberOfColumns; j ++) {
        if (j + 1 >= matrixElement.dataList[i].size()) {
          matrix.elements[i][j].assignValue(*this->owner, 0);
          continue;
        }
        matrix.elements[i][j] = matrixElement.dataList[i][j + 1];
      }
    }
    matrixElement.data.assignMatrixExpressions(matrix, *this->owner, true, true);
  } else {
    matrixElement.data.makeMatrix(nullptr, *this->owner);
  }
  matrixElement.dataList.setSize(0);
  matrixElement.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Calculator::replaceMatrixXByE]";
  }
  return this->decreaseStackSetCharacterRanges(1);
}

bool CalculatorParser::replaceMatrixEXByMatrixNewRow() {
  SyntacticElement& matrixElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  matrixElt.dataList.lastObject()->addChildOnTop(element.data);
  matrixElt.dataList.setSize(matrixElt.dataList.size + 1);
  matrixElt.dataList.lastObject()->makeSequence(*this->owner);
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Calculator::replaceMatrixEXByMatrixNewRow]";
  }
  return this->decreaseStackSetCharacterRanges(2);
}

bool CalculatorParser::replaceMatrixEXByMatrix() {
  SyntacticElement& matrixElement = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  matrixElement.dataList.lastObject()->addChildOnTop(element.data);
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceMatrixEXByMatrix]";
  }
  return this->decreaseStackSetCharacterRanges(2);
}

bool CalculatorParser::replaceMatrixEXByMatrixX() {
  SyntacticElement& matrixElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  matrixElt.dataList.lastObject()->addChildOnTop(element.data);
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceMatrixEXByMatrixX]";
  }
  return this->decreaseStackExceptLast(1);
}

bool CalculatorParser::replaceAXbyEX() {
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  element.controlIndex = this->conExpression();
  return true;
}

bool CalculatorParser::replaceAXXbyEXX() {
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  element.controlIndex = this->conExpression();
  return true;
}

bool CalculatorParser::replaceIntegerXbyEX() {
  SyntacticElement& element = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  element.controlIndex = this->conExpression();
  Rational value;
  value.assignString(element.data.getValue<std::string>());
  element.data.assignValue(*this->owner, value);
  return true;
}

std::string CalculatorParser::toStringIsCorrectAsciiCalculatorString(const std::string& input) {
  std::stringstream out;
  HashedList<unsigned char, HashFunctions::hashFunction> badCharacters;
  for (unsigned i = 0; i < input.size(); i ++) {
    unsigned char next = static_cast<unsigned char>(input[i]);
    if (!this->isStandardCalculatorCharacter(next)) {
      badCharacters.addOnTopNoRepetition(next);
    }
  }
  if (badCharacters.size > 0) {
    out << "Non-ascii characters detected in your input, ";
    List<std::string> hexCodes;
    for (int i = 0; i < badCharacters.size; i ++) {
      hexCodes.addOnTop(StringRoutines::convertByteToHex(badCharacters[i]));
    }
    out << "with byte values: " << hexCodes.toStringCommaDelimited() << ". ";
    out << "Perhaps you copy+pasted from webpage/pdf file or are using non-English keyboard setup? ";
  }
  return out.str();
}

bool CalculatorParser::isStandardCalculatorCharacter(unsigned char input) {
  if (10 <= input && input <= 126) {
    return true;
  }
  if (this->isInterpretedAsEmptySpace(input)) {
    return true;
  }
  return false;
}

bool CalculatorParser::allowsApplyFunctionInPreceding(const std::string& lookAhead) {
  return
  lookAhead != "{" && lookAhead != "_" &&
  lookAhead != "\\circ" && lookAhead != "{}" &&
  lookAhead != "$";
}

bool CalculatorParser::replaceSequenceUXEYBySequenceZY(int controlIndex) {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& afterleft = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  left.data.addChildOnTop(right.data);
  left.controlIndex = controlIndex;
  afterleft = *this->currentSyntacticStack->lastObject();
  this->decreaseStackExceptLast(2);
  return true;
}

bool CalculatorParser::replaceYXBySequenceX(int controlIndex) {
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceYXBySequenceX]";
  }
  return this->replaceYXdotsXBySequenceYXdotsX(controlIndex, 1);
}

bool CalculatorParser::replaceYBySequenceY(int controlIndex) {
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceYBySequenceY]";
  }
  return this->replaceYXdotsXBySequenceYXdotsX(controlIndex, 0);
}

bool CalculatorParser::replaceXXYBySequenceY(int controlIndex) {
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceXXYBySequenceY]";
  }
  this->replaceYBySequenceY(controlIndex);
  return this->replaceXXYByY();
}

bool CalculatorParser::replaceXXYXBySequenceYX(int controlIndex) {
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceXXYXBySequenceYX]";
  }
  this->replaceYXdotsXBySequenceYXdotsX(controlIndex, 1);
  return this->replaceXXYXByYX();
}

bool CalculatorParser::replaceSequenceXEBySequence(int controlIndex) {
  SyntacticElement& left =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  left.data.addChildOnTop(right.data);
  left.controlIndex = controlIndex;
  this->decreaseStackSetCharacterRanges(2);
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceSequenceXEBySequence]";
  }
  return true;
}

bool CalculatorParser::replaceYXdotsXBySequenceYXdotsX(int controlIndex, int numberOfXs) {
  SyntacticElement& main = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - numberOfXs - 1];
  Expression newExpr;
  newExpr.reset(*this->owner, 2);
  newExpr.addChildAtomOnTop(this->owner->opSequence());
  newExpr.addChildOnTop(main.data);
  main.data = newExpr;
  main.controlIndex = controlIndex;
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceYXdotsXBySequenceYXdotsX]";
  }
  return true;
}

bool CalculatorParser::replaceEXEBySequence(int controlIndex) {
  SyntacticElement& left =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(this->owner->opSequence());
  newExpr.addChildOnTop(left.data);
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  left.controlIndex = controlIndex;
  this->decreaseStackSetCharacterRanges(2);
  return true;
}

bool CalculatorParser::replaceEEBy_CofEE(int controlIndex) {
  SyntacticElement& left =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(controlIndex));
  newExpr.addChildOnTop(left.data);
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  this->decreaseStackSetCharacterRanges(1);
  return true;
}

bool CalculatorParser::replaceEOXbyEX() {
  SyntacticElement& left =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& operationElement = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  int operation = this->getOperationIndexFromControlIndex(operationElement.controlIndex);
  Expression newExpr;
  newExpr.reset(*this->owner, 2);
  newExpr.addChildAtomOnTop(operation);
  newExpr.addChildOnTop(left.data);
  left.data = newExpr;
  this->decreaseStackExceptLast(1);
  return true;
}

bool CalculatorParser::replaceVbyVdotsVAccordingToPredefinedWordSplits() {
  MacroRegisterFunctionWithName("Calculator::replaceVbyVdotsVAccordingToPredefinedWordSplits");
  SyntacticElement& currentElement = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  const std::string& currentVar = this->owner->operations.keys[currentElement.data.data];
  if (!this->predefinedWordSplits.contains(currentVar)) {
    global.fatal << "Predefined word splits array does not contain the variable: " << currentElement.data.toString()
    << ". This should not happen in the body of this function. " << global.fatal;
  }
  List<std::string>& split = this->predefinedWordSplits.getValueCreateEmpty(currentVar);
  SyntacticElement newElt;
  this->popTopSyntacticStack();
  *(this->owner) << "Predefined symbol replacement: replacing "
  << currentVar << " with the sequence of symbols " << split.toStringCommaDelimited()
  << ". If you do not want such replacements to take "
  << "place you should add the %DontUsePredefinedWordSplits option "
  << "at the start of your input. "
  << "The predefined symbol replacements are used to guard "
  << "the user from accidental typos such as confusing "
  << "x y (the product of x and y) with xy "
  << "(a single variable whose name contains the letters x and y). "
  ;
  for (int i = 0; i < split.size; i ++) {
    newElt.data.makeAtom(*this->owner, this->owner->addOperationNoRepetitionOrReturnIndexFirst(split[i]));
    newElt.controlIndex = this->controlSequences.getIndex(split[i]);
    if (newElt.controlIndex == - 1) {
      newElt.controlIndex = this->conVariable();
    }
    (*this->currentSyntacticStack).addOnTop(newElt);
  }
  return true;
}

bool CalculatorParser::replaceEEXBy_CofEE_X(int controlIndex) {
  SyntacticElement& left =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceEEXBy_CofEE_X]";
  }
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(controlIndex));
  newExpr.addChildOnTop(left.data);
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  this->decreaseStackExceptLast(1);
  return true;
}

bool CalculatorParser::replaceEXXEXEBy_CofEEE(int controlIndex) {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 6];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this->owner, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(controlIndex));
  newExpr.addChildOnTop(left.data);
  newExpr.addChildOnTop(middle.data);
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  this->decreaseStackSetCharacterRanges(5);
  return true;
}

bool CalculatorParser::replaceEXXEXEXBy_CofEEE_X(int controlIndex) {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 7];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this->owner, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(controlIndex));
  newExpr.addChildOnTop(left.data);
  newExpr.addChildOnTop(middle.data);
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  this->decreaseStackExceptLast(5);
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceEXXEXEXBy_CofEEE_X]";
  }
  return true;
}

bool CalculatorParser::replaceUnderscoreEPowerEbyLimits() {
  SyntacticElement& bottom =
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& top =
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(this->owner->opLimitBoundary());
  newExpr.addChildOnTop(bottom.data);
  newExpr.addChildOnTop(top.data);
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4].data = newExpr;
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4].controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceUnderscoreEPowerEbyLimits]";
  }
  return this->decreaseStackSetCharacterRanges(3);
}

bool CalculatorParser::replacePowerEUnderScoreEbyLimits() {
  SyntacticElement& bottom =
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& top =
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  MathRoutines::swap(bottom, top);
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replacePowerEUnderScoreEbyLimits]";
  }
  return this->replaceUnderscoreEPowerEbyLimits();
}

bool CalculatorParser::replaceXEXEXBy_CofEE(int operation) {
  SyntacticElement& lefT = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& result = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(operation));
  newExpr.addChildOnTop(lefT.data);
  newExpr.addChildOnTop(right.data);
  result.data = newExpr;
  result.controlIndex = this->conExpression();
  this->decreaseStackSetCharacterRanges(4);
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceXEXEXBy_CofEE]";
  }
  return true;
}

bool CalculatorParser::replaceEXdotsXbySsXdotsX(int numberOfDots) {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - numberOfDots - 1];
  bool found = false;
  for (int i = (*this->currentSyntacticStack).size - numberOfDots - 2; i >= 0; i --) {
    SyntacticElement& current = (*this->currentSyntacticStack)[i];
    if (current.numBoundVariablesInherited >= 0 && current.numNonBoundVariablesInherited >= 0) {
      this->nonBoundVariablesInContext.setSize(current.numNonBoundVariablesInherited);
      this->boundVariablesInContext.setSize(current.numBoundVariablesInherited);
      found = true;
      break;
    }
  }
  if (!found) {
    this->nonBoundVariablesInContext.clear();
    this->boundVariablesInContext.clear();
  }
  Expression newExpr;
  newExpr.reset(*this->owner);
  newExpr.addChildAtomOnTop(this->owner->opCommandSequence());
  newExpr.addChildOnTop(left.data);
  left.data = newExpr;
  left.controlIndex = this->conSequenceStatements();
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceEXdotsXbySsXdotsX]";
  }
  return true;
}

bool CalculatorParser::replaceSsSsXdotsXbySsXdotsX(int numberOfDots) {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - numberOfDots - 2];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - numberOfDots - 1];
  if (!left.data.startsWith(this->owner->opCommandSequence())) {
    global.fatal << "replaceSsSsXdotsXbySsXdotsX called but left expression is not EndStatement." << global.fatal;
  }
  left.data.setExpectedSize(left.data.size() + right.data.size() - 1);
  for (int i = 1; i < right.data.size(); i ++) {
    left.data.addChildOnTop(right.data[i]);
  }
  left.controlIndex = this->conSequenceStatements();
  (*this->currentSyntacticStack).popIndexShiftDown((*this->currentSyntacticStack).size - numberOfDots - 1);
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceSsSsXdotsXbySsXdotsX]";
  }
  return true;
}

bool CalculatorParser::replaceEXEByCofEE(int operation) {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(operation));
  newExpr.addChildOnTop(left.data);
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  this->decreaseStackSetCharacterRanges(2);
  return true;
}

bool CalculatorParser::replaceEXEXByEX() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this->owner, 2);
  newExpr.addChildOnTop(left.data);
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  this->decreaseStackExceptLast(2);
  return true;
}

bool CalculatorParser::replaceXXbyO(int operation) {
  SyntacticElement& result = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  result.data.makeAtom(*this->owner, operation);
  result.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceXXbyO]";
  }
  return this->decreaseStackSetCharacterRanges(1);
}

bool CalculatorParser::replaceXXYbyOY(int operation) {
  SyntacticElement& result = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  result.data.makeAtom(*this->owner, operation);
  result.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceXXYbyOY]";
  }
  return this->decreaseStackExceptLast(1);
}

bool CalculatorParser::replaceEPowerMinusEXByEX() {
  SyntacticElement& base = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  SyntacticElement& minusExponent = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceEPowerMinusEXByEX]";
  }
  Expression exponent;
  exponent.makeOX(*this->owner, this->owner->opMinus(), minusExponent.data);
  Expression incoming;
  incoming.makeXOX(*this->owner, this->owner->opPower(), base.data, exponent);
  base.data = incoming;
  return this->decreaseStackExceptLast(3);
}

bool CalculatorParser::replaceEOEXByEX() {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceEOEXByEX]";
  }
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.addChildOnTop(left.data);
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  middle = *(*this->currentSyntacticStack).lastObject();
  this->decreaseStackExceptLast(2);
  return true;
}

bool CalculatorParser::replaceEXEXBy_OofEE_X(int operation) {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceEOEXByEX]";
  }
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(operation);
  newExpr.addChildOnTop(left.data);
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  middle = *(*this->currentSyntacticStack).lastObject();
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceEXEXBy_OofEE_X]";
  }
  return this->decreaseStackExceptLast(2);
}

bool CalculatorParser::replaceC1C2Eby_C2ofC1E() {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceC1C2Eby_C2ofC1E]";
  }
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(left.controlIndex));
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  left.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(2);
}

bool CalculatorParser::replaceXEEXBy_OofEE_X(int inputOperation) {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& left  =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(inputOperation);
  newExpr.addChildOnTop(middle.data);
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  left.controlIndex = this->conExpression();
  middle = *(*this->currentSyntacticStack).lastObject();
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceXEEXBy_OofEE_X]";
  }
  return this->decreaseStackExceptLast(2);
}

bool CalculatorParser::replaceXEEBy_OofEE(int inputOperation) {
  SyntacticElement& right =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& left  =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  Expression newExpr;
  newExpr.reset(*this->owner, 3);
  newExpr.addChildAtomOnTop(inputOperation);
  newExpr.addChildOnTop(middle.data);
  newExpr.addChildOnTop(right.data);
  left.data = newExpr;
  left.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceXEEBy_OofEE]";
  }
  return this->decreaseStackSetCharacterRanges(2);
}

bool CalculatorParser::isSeparatorFromTheLeftGeneral(const std::string& input) {
  return input == "{" || input == "(" || input == "[" ||
  input == "," || input == ":" || input == ";" ||
  input == " " || input == "Matrix" ||
  input == "&";
}

bool CalculatorParser::isSeparatorFromTheLeftForInterval(const std::string& input) {
  return input == "," || input == "(" || input == "[" || input == ":" || input == ";" ||
  input == "=" || input == "==" || input == "\\\\" || input == "\\cup" || input == "\\cap" ||
  input == "\\end" || input == "&" || input == "EndProgram";
}

bool CalculatorParser::isSeparatorFromTheRightGeneral(const std::string& input) {
  return input == "}" || input == ")" || input == "]" || input == "," || input == ":" || input == ";" ||
  input == "Matrix" ||
  input == "=" || input == "\\\\" ||
  input == "\\end" || input == "&" || input == "EndProgram";
}

bool CalculatorParser::isSeparatorFromTheLeftForList(const std::string& input) {
  return input == "{" || input == "(" || input == "[" || input == ":" || input == ";" || input == " " ||
  input == "Matrix";
}

bool CalculatorParser::isSeparatorFromTheRightForList(const std::string& input) {
  return input == "}" || input == ")" || input == "]" || input == ":" || input == ";" ||
  input == "Matrix" || input == "\\\\" || input == "\\end";
}

bool CalculatorParser::isSeparatorFromTheLeftForDefinition(const std::string& input) {
  return input == "{" || input == "(" || input == ";" || input == "," ||
  input == "SequenceStatements" || input == " " ||
  input == "or" ||
  input == "and";
}

bool CalculatorParser::isSeparatorFromTheRightForDefinition(const std::string& input) {
  return input == "}" || input == ")" || input == ";" || input == "," ||
  input == "EndProgram" || input == "or" ||
  input == "and"
  ;
}

bool CalculatorParser::allowsTensorInPreceding(const std::string& lookAhead) {
  return this->allowsTimesInPreceding(lookAhead) || lookAhead == "\\otimes";
}

bool CalculatorParser::allowsTimesInNext(const std::string& preceding) {
  if (
    preceding == "{}" || preceding == "^" || preceding == "\\sqrt" || preceding == "_"
  ) {
    return false;
  }
  return true;
}

bool CalculatorParser::allowsTimesInPreceding(const SyntacticElement& preceding, const std::string& lookAhead) {
  if (preceding.controlIndex == this->conExpression()) {
    if (preceding.data.isBuiltInAtom()) {
      return lookAhead == "+" || lookAhead == "-" || lookAhead == "*" || lookAhead == "/" ||
      lookAhead == "\\cup" ||
      lookAhead == ")" || lookAhead == "]" || lookAhead == "}" ||
      lookAhead == "=" ||
      lookAhead == ">" || lookAhead == "<" ||
      lookAhead == "\\leq" || lookAhead == "\\geq" ||
      lookAhead == "," || lookAhead == ";" ||
      lookAhead == ":" || lookAhead == "&" || lookAhead == "Matrix" || lookAhead == "\\" ||
      lookAhead == "or" ||
      lookAhead == "and" ||
      lookAhead == "EndProgram"
      ;
    }
  }
  return this->allowsTimesInPreceding(lookAhead);
}

bool CalculatorParser::allowsTimesInPreceding(const std::string& lookAhead) {
  return lookAhead == "+" || lookAhead == "-" || lookAhead == "*" || lookAhead == "/" ||
  this->owner->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(lookAhead) ||
  lookAhead == "\\frac" ||
  lookAhead == "Expression" ||  lookAhead == "Integer" || lookAhead == "\\cup" ||
  lookAhead == "(" || lookAhead == "[" ||
  lookAhead == "|" ||
  lookAhead == ")" || lookAhead == "]" || lookAhead == "}" ||
  lookAhead == "=" ||
  lookAhead == ">" || lookAhead == "<" ||
  lookAhead == "\\geq" || lookAhead == "\\leq" ||
  lookAhead == "Variable" || lookAhead == "," || lookAhead == ";" ||
  lookAhead == ":" || lookAhead == "&" || lookAhead == "MatrixEnd" ||
  lookAhead == "or" ||
  lookAhead == "and" ||
  lookAhead == "\\" ||
  lookAhead == "sqrt" || lookAhead == "\\sqrt" || lookAhead == "EndProgram"
  ;
}

bool CalculatorParser::allowsPowerInPreceding(const std::string& lookAhead) {
  return lookAhead != "{}";
}

bool CalculatorParser::allowsPowerInNext(const std::string& lookBehind) {
  return lookBehind != "_";
}

bool CalculatorParser::allowsLimitProcessInPreceding(const std::string& lookAhead) {
  return lookAhead == "and" || lookAhead == "or" ||
  lookAhead == "(" || lookAhead == "[" ||
  lookAhead == ")" || lookAhead == "]" || lookAhead == "}" ||
  lookAhead == "," || lookAhead == ";" ||
  lookAhead == ":" || lookAhead == "&" || lookAhead == "Matrix" ||
  lookAhead == "\\" ||
  lookAhead == "EndProgram";
}

bool CalculatorParser::allowsAndInPreceding(const std::string& lookAhead) {
  return lookAhead == "and" || lookAhead == "or" ||
  lookAhead == "(" || lookAhead == "[" ||
  lookAhead == ")" || lookAhead == "]" || lookAhead == "}" ||
  lookAhead == "," || lookAhead == ";" ||
  lookAhead == ":" || lookAhead == "&" || lookAhead == "Matrix" ||
  lookAhead == "\\" ||
  lookAhead == "EndProgram"
  ;
}

bool CalculatorParser::allowsOrInPreceding(const std::string& lookAhead) {
  return lookAhead == "and" || lookAhead == "or" ||
  lookAhead == "(" || lookAhead == "[" ||
  lookAhead == ")" || lookAhead == "]" || lookAhead == "}" ||
  lookAhead == "," || lookAhead == ";" ||
  lookAhead == ":" || lookAhead == "&" || lookAhead == "Matrix" ||
  lookAhead == "\\" ||
  lookAhead == "EndProgram"
  ;
}

bool CalculatorParser::allowsInInPreceding(const std::string& lookAhead) {
  return this->allowsIfInPreceding(lookAhead);
}

bool CalculatorParser::allowsIfInPreceding(const std::string& lookAhead) {
  return
  lookAhead == ")" || lookAhead == "]" || lookAhead == "}" ||
  lookAhead == "," || lookAhead == ";" ||
  lookAhead == "+" || lookAhead == "-" ||
  lookAhead == "/" || lookAhead == "*" ||
  lookAhead == ":" || lookAhead == "&" || lookAhead == "Matrix" ||
  lookAhead == "\\" ||
  lookAhead == "EndProgram"
  ;
}

bool CalculatorParser::allowsPlusInPreceding(const std::string& lookAhead) {
  return lookAhead == "+" || lookAhead == "-" || lookAhead == "," ||
  lookAhead == "=" || lookAhead == "<" || lookAhead == ">" ||
  lookAhead == "\\geq" || lookAhead == "\\leq" ||
  lookAhead == ")" || lookAhead == ";" || lookAhead == "]" || lookAhead == "}" ||
  lookAhead == ":" || lookAhead == "," || lookAhead == "\\choose" ||
  lookAhead == "or" ||
  lookAhead == "and" ||
  lookAhead == "EndProgram" ||
  lookAhead == "&" || lookAhead == "MatrixEnd" || lookAhead == "\\";
  ;
}

bool CalculatorParser::allowsDivideInPreceding(const std::string& lookAhead) {
  return this->allowsTimesInPreceding(lookAhead);
}

bool CalculatorParser::extractExpressions(Expression& outputExpression, std::string* outputErrors) {
  MacroRegisterFunctionWithName("Calculator::extractExpressions");
  //std::string lookAheadToken;
  std::stringstream errorLog;
  (*this->currentSyntacticStack).reserve((*this->currrentSyntacticSoup).size + this->numberOfEmptyTokensStart);
  (*this->currentSyntacticStack).setSize(this->numberOfEmptyTokensStart);
  for (int i = 0; i < this->numberOfEmptyTokensStart; i ++) {
    (*this->currentSyntacticStack)[i] = this->getEmptySyntacticElement();
  }
  this->parsingLog = "";
  this->nonBoundVariablesInContext.clear();
  this->boundVariablesInContext.clear();
  const int maxNumTimesOneRuleCanBeCalled = 1000;
  int counterReport = 0;
  int symbolsToIssueReport = 100;
  int minMillisecondsPerReport = 200;
  this->owner->statistics.startParsing = global.getElapsedMilliseconds();
  this->owner->statistics.lastStopwatchParsing = this->owner->statistics.startParsing;
  ProgressReport report;
  for (
    this->counterInSyntacticSoup = 0;
    this->counterInSyntacticSoup < (*this->currrentSyntacticSoup).size;
    this->counterInSyntacticSoup ++
  ) {
    counterReport ++;
    if (counterReport >= symbolsToIssueReport) {
      counterReport = 0;
      int64_t currentMilliseconds = global.getElapsedMilliseconds();
      if (currentMilliseconds - this->owner->statistics.lastStopwatchParsing > minMillisecondsPerReport) {
        this->owner->statistics.lastStopwatchParsing = currentMilliseconds;
        std::stringstream reportStream;
        reportStream << "Processed " << this->counterInSyntacticSoup << " out of " << (*this->currrentSyntacticSoup).size
        << " syntactic elements. ";
        report.report(reportStream.str());
      }
    }
    (*this->currentSyntacticStack).addOnTop((*this->currrentSyntacticSoup)[this->counterInSyntacticSoup]);
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "[next token]";
      this->logParsingOperation();
    }
    int numTimesRulesCanBeAppliedWithoutStackDecrease = 0;
    int minStackSize = this->currentSyntacticStack->size ;
    while (this->applyOneRule()) {
      this->logParsingOperation();
      if (this->currentSyntacticStack->size < minStackSize) {
        numTimesRulesCanBeAppliedWithoutStackDecrease = 0;
        minStackSize = this->currentSyntacticStack->size ;
      } else {
        numTimesRulesCanBeAppliedWithoutStackDecrease ++;
      }
      if (numTimesRulesCanBeAppliedWithoutStackDecrease > maxNumTimesOneRuleCanBeCalled) {
        global.fatal << "This may be a programming error: Calculator::applyOneRule called more than "
        << maxNumTimesOneRuleCanBeCalled
        << " times without advancing to the next syntactic element in the syntactic soup. "
        << "If this is indeed an expression which requires that "
        << "many application of a single parsing rule, "
        << "then you should modify function Calculator::extractExpressions. "
        << global.fatal;
      }
    }
  }
  bool success = false;
  if ((*this->currentSyntacticStack).size == this->numberOfEmptyTokensStart) {
    errorLog << "Non-meaningful/empty input detected (spacebar, enter characters only?).";
  } else if ((*this->currentSyntacticStack).size == this->numberOfEmptyTokensStart + 1) {
    SyntacticElement& result = (*this->currentSyntacticStack)[this->numberOfEmptyTokensStart];
    if (result.errorString == "" && result.controlIndex == this->conExpression()) {
      outputExpression = result.data;
      success = true;
    } else if (result.errorString != "") {
      errorLog << "Syntax error with message: " << result.errorString;
    } else {
      errorLog << "Syntax error: your command simplifies to a single syntactic element but it is not an expression. <br>";
      errorLog << "It simplifies to:<br> " << this->toStringSyntacticStackHTMLTable(false, false);
    }
  } else {
    errorLog << "Syntax error: your command does not simplify to a single syntactic element. <br>";
    errorLog << "Instead it simplifies to:<br> " << this->toStringSyntacticStackHTMLTable(false, false);
  }
  if (outputErrors != nullptr) {
    *outputErrors = errorLog.str();
  }
  return success;
}

void CalculatorParser::logParsingOperation() {
  if (!this->flagLogSyntaxRules) {
    return;
  }
  this->parsingLog.addOnTop("<hr>");
  this->parsingLog.addOnTop(this->toStringSyntacticStackHTMLTable(false, false));
  this->parsingLog.addOnTop("[" + this->lastRuleName + "]");
}

bool CalculatorParser::applyOneRule() {
  MacroRegisterFunctionWithName("CalculatorParser::applyOneRule");
  if (this->currentSyntacticStack->size <= this->numberOfEmptyTokensStart) {
    return false;
  }
  const SyntacticElement& lastE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  const std::string& lastS = this->controlSequences[lastE.controlIndex];
  const SyntacticElement& secondToLastE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  const std::string& secondToLastS = this->controlSequences[secondToLastE.controlIndex];
  if (secondToLastS == "Integer" && lastS != "Integer" && lastS != ".") {
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "digit to number";
    }
    return this->replaceIntegerXbyEX();
  }
  if (
    lastS == " " &&
    secondToLastS == "\\" &&
    this->currentSyntacticStack->size >= this->numberOfEmptyTokensStart + 2
  ) {
    this->popTopSyntacticStack();
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "[Rule: remove \\ ]";
    }
    return this->popTopSyntacticStack();
  }
  if (lastS == " " || lastS == "~") {
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "remove white space";
    }
    return this->popTopSyntacticStack();
  }
  if (lastS == "(" || lastS == "{") {
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1].numNonBoundVariablesInherited = this->nonBoundVariablesInContext.size;
    (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1].numBoundVariablesInherited = this->boundVariablesInContext.size;
  }
  const SyntacticElement& thirdToLastE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  const std::string& thirdToLastS = this->controlSequences[thirdToLastE.controlIndex];
  const SyntacticElement& fourthToLastE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  const std::string& fourthToLastS = this->controlSequences[fourthToLastE.controlIndex];
  const SyntacticElement& fifthToLastE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  const std::string& fifthToLastS = this->controlSequences[fifthToLastE.controlIndex];
  const SyntacticElement& sixthToLastE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 6];
  const std::string& sixthToLastS = this->controlSequences[sixthToLastE.controlIndex];
  const SyntacticElement& seventhToLastE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 7];
  const std::string& seventhToLastS = this->controlSequences[seventhToLastE.controlIndex];
  const SyntacticElement& eighthToLastE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 8];
  const std::string& eighthToLastS = this->controlSequences[eighthToLastE.controlIndex];
  if (secondToLastS == "%" && lastS == "LogParsing") {
    this->flagLogSyntaxRules = true;
    this->lastRuleName = "[Rule: request parsing log]";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "LogFull") {
    this->owner->comments
    << "<hr>Requested a full log of the evaluation process.<hr>";
    this->owner->flagLogFullTreeCrunching = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "UseBracketForIntervals") {
    this->owner->flagUseBracketsForIntervals = true;
    this->owner->comments << "Using brackets for intervals. ";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "DontUsePredefinedWordSplits") {
    this->owner->flagUsePredefinedWordSplits = false;
    this->owner->comments << "Using predefined word splits -for example xy is replaced by x y. ";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "WriteLatexDetails") {
    this->owner->flagWriteLatexPlots = true;
    this->owner->comments << "Creating LaTeX files.";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "PlotShowJavascriptOnly") {
    this->owner->flagPlotShowJavascriptOnly = true;
    this->owner->comments << "Plots show javascript only. ";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "PlotDetails") {
    this->owner->flagPlotNoControls = false;
    this->owner->comments << "Plot details on. ";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "HidePolynomialDataStructure") {
    this->owner->flagHidePolynomialBuiltInTypeIndicator = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "LogEvaluation") {
    this->owner->flagLogEvaluation = true;
    *(this->owner) << "Log evaluation start. ";
    this->owner->logTime(this->owner->statistics.lastStopwatchParsing);
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "UseLnInsteadOfLog") {
    this->owner->comments << "Displaying complex logarithms as ln. ";
    this->owner->flagUseLnInsteadOfLog = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "UseLnAbsInsteadOfLog") {
    this->owner->flagUseLnAbsInsteadOfLogForIntegrationNotation = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "NumberColors") {
    if (!this->owner->flagUseNumberColors) {
      *this->owner << "<span style ='color:blue'>Floating point numbers</span> are displayed in "
      << "<span style='color:blue'>blue</span>."
      << "<br><span style='color:red'>Algebraic numbers</span> "
      << "are displayed in <span style='color:red'>red</span>. "
      << "<br>Rational numbers are displayed in default color.";
    }
    this->owner->flagUseNumberColors = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "SequenceStatements") {
    this->lastRuleName = "comment out command";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (thirdToLastS == "%" && secondToLastS == "SequenceStatements") {
    this->lastRuleName = "comment out command below top";
    this->popBelowStackTop();
    return this->popBelowStackTop();
  }
  if (secondToLastS == "%" && lastS == "LogCache") {
    this->owner->flagLogCache = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "LogRules") {
    this->owner->flagLogRules = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "ShowContext") {
    this->owner->flagDisplayContext = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "NoLogarithmExponentShortcut") {
    this->owner->atomsWhoseExponentsAreInterpretedAsFunctions.
    removeFirstOccurenceSwapWithLast(std::string("\\log"));
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "FullTree") {
    this->owner->flagDisplayFullExpressionTree = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "HideLHS") {
    this->owner->flagHideLHS = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "NoFrac") {
    this->owner->flagUseFracInRationalLaTeX = false;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }

  if (
    secondToLastS == "\\int" &&
    lastS == "Expression"
  ) {
    return this->replaceOXbyEX();
  }
  if (
    thirdToLastS == "\\int" &&
    secondToLastS == "Expression"
  ) {
    return this->replaceOXXByEXX();
  }
  if (
    secondToLastS == "\\int_{*}^{**}" &&
    lastS == "Expression"
  ) {
    return this->replaceAXbyEX();
  }
  if (
    thirdToLastS == "\\int_{*}^{**}" &&
    secondToLastS == "Expression"
  ) {
    return this->replaceAXXbyEXX();
  }
  if (
    this->isDefiniteIntegral(thirdToLastS) &&
    secondToLastS == "Expression" &&
    this->canBeRegardedAsDifferentialForm(lastE)
  ) {
    return this->setStackValue(thirdToLastE.data, "Expression", - 3);
  }
  if (
    (fifthToLastS == "\\int" || this->isDefiniteIntegral(fifthToLastS)) &&
    fourthToLastS == "Expression" &&
    (thirdToLastS == "+" || thirdToLastS == "-") &&
    secondToLastS == "Expression" &&
    this->canBeRegardedAsDifferentialForm(lastE)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    fourthToLastS == "\\int" &&
    thirdToLastS == "_" &&
    secondToLastS == "Expression"
  ) {
    Expression integralAtom;
    integralAtom.makeAtom(*this->owner, "\\int");
    Expression underscore;
    underscore.makeXOX(*this->owner, this->owner->opUnderscore(), integralAtom, secondToLastE.data);
    this->setStackValue(underscore, "\\int_{*}", - 4);
    return this->decreaseStackExceptLast(2);
  }
  if (
    fourthToLastS == "\\int" &&
    thirdToLastS == "^" &&
    secondToLastS == "Expression"
  ) {
    Expression integralAtom;
    integralAtom.makeAtom(*this->owner, "\\int");
    Expression underscore;
    underscore.makeXOX(*this->owner, this->owner->opPower(), integralAtom, secondToLastE.data);
    this->lastRuleName = "\\int^{*}";
    this->setStackValue(underscore, "\\int^{*}", - 4);
    return this->decreaseStackExceptLast(2);
  }
  if (
    thirdToLastS == "\\int_{*}" &&
    secondToLastS == "^" &&
    lastS == "Expression"
  ) {
    Expression exponent;
    exponent.makeXOX(*this->owner, this->owner->opPower(), thirdToLastE.data, lastE.data);
    this->setStackValue(exponent, "\\int_{*}^{**}", - 3);
    this->lastRuleName = "int_{*}^{**}";
    return this->decreaseStack(2);
  }
  if (
    fourthToLastS == "\\int_{*}" &&
    thirdToLastS == "^" &&
    secondToLastS == "Expression"
  ) {
    Expression exponent;
    exponent.makeXOX(*this->owner, this->owner->opPower(), fourthToLastE.data, secondToLastE.data);
    this->setStackValue(exponent, "\\int_{*}^{**}", - 4);
    this->lastRuleName = "int_{*} ^ {**} --> int_{*}^{**}";
    return this->decreaseStackExceptLast(2);
  }
  if (
    thirdToLastS == "\\int^{*}" &&
    secondToLastS == "_" &&
    lastS == "Expression"
  ) {
    Expression underscore;
    underscore.makeXOX(*this->owner, this->owner->opUnderscore(), thirdToLastE.data, lastE.data);
    this->lastRuleName = "[\\int^{*}_{**}]";
    this->setStackValue(underscore, "\\int_{*}^{**}", - 3);
    return this->decreaseStack(2);
  }
  if (
    fourthToLastS == "\\int^{*}" &&
    thirdToLastS == "_" &&
    secondToLastS == "Expression"
  ) {
    Expression underscore;
    underscore.makeXOX(*this->owner, this->owner->opUnderscore(), fourthToLastE.data, secondToLastE.data);
    this->lastRuleName = "\\int^{*} _ Expression --> \\int^{*}_{**}";
    this->setStackValue(underscore, "\\int_{*}^{**}", - 4);
    return this->decreaseStackExceptLast(2);
  }

  if (this->owner->flagUsePredefinedWordSplits) {
    if (lastS == "Variable") {
      if (this->predefinedWordSplits.contains(this->owner->operations.keys[lastE.data.data])) {
        return this->replaceVbyVdotsVAccordingToPredefinedWordSplits();
      }
    }
  }
  if (secondToLastS == "=" && lastS == ":") {
    return this->replaceXXByCon(this->conIsDenotedBy());
  }
  if (secondToLastS == "{" && lastS == "}") {
    return this->replaceXXByCon(this->conApplyFunction());
  }
  if (lastS == "\\cdot") {
    return this->replaceXByCon(this->conTimes());
  }
  if (lastS == "\\varnothing") {
    return this->replaceXByO(this->owner->opEmptySet());
  }
  if (lastS == "\\circ" && secondToLastS != "{" && secondToLastS != "(" && secondToLastS != "^") {
    return this->replaceXByCon(this->conApplyFunction());
  }
  if (fifthToLastS == "{" && fourthToLastS == "{" && thirdToLastS == "Variable" && secondToLastS == "}" && lastS == "}") {
    return this->replaceXXVXdotsXbyE_BOUND_XdotsX(2);
  }
  if (secondToLastS == "Variable" && ((lastS != "}" && lastS != " ") || thirdToLastS != "{" || fourthToLastS != "{")) {
    return this->replaceVXdotsXbyE_NONBOUND_XdotsX(1);
  }
  if (thirdToLastS == "Variable" && ((secondToLastS != "}" && secondToLastS != " ") || fourthToLastS != "{" || fifthToLastS != "{")) {
    return this->replaceVXdotsXbyE_NONBOUND_XdotsX(2);
  }
  if (fourthToLastS == "{" && thirdToLastS == "Variable" && secondToLastS == "}" && lastS != "}" && lastS != " ") {
    return this->replaceVXdotsXbyE_NONBOUND_XdotsX(2);
  }
  if ((secondToLastS == "\\left" || secondToLastS == "\\right") && lastS == "|") {
    return this->replaceXYByY();
  }
  if (secondToLastS == "\\left" && (lastS == "(" || lastS == "[" || lastS == "{")) {
    return this->replaceXYByY();
  }
  if (secondToLastS == "\\right" && (lastS == ")" || lastS == "]" || lastS == "}")) {
    return this->replaceXYByY();
  }
  if (lastS == "=" && secondToLastS == "=") {
    return this->replaceXXByCon(this->conEqualEqual());
  }
  if (secondToLastS == "(" && lastS == ")") {
    return this->replaceXXByEEmptySequence();
  }
  if (secondToLastS == "\\text" && lastS == "d") {
    if (lastE.data.isOperationGiven("d")) {
      return this->replaceXXbyO(this->owner->opDifferential());
    }
  }
  if (thirdToLastS == "\\text" && secondToLastS == "Expression") {
    if (secondToLastE.data.isOperationGiven("d")) {
      return this->replaceXXYbyOY(this->owner->opDifferential());
    }
  }
  if (thirdToLastS == "Integer" && secondToLastS == "." && lastS == "Integer") {
    return this->replaceIntegerDotIntegerByE();
  }
  // there is an ambiguity on how should function application be associated
  // Which is better: x{}y{}z = x{} (y{}z), or x{}y{}z = (x{}y){}z ?
  // In our implementation, we choose x{}y{}z= x{} (y{}z). Although this is slightly harder to implement,
  // it appears to be the more natural choice.
  // if (fourthToLastS == "Expression" && thirdToLastS == "{}" && secondToLastS == "Expression"
  //     && this->allowsApplyFunctionInPreceding(lastS) )
  //   return this->replaceEOEXByEX(secondToLastE.data.format);
  if (
    fourthToLastS == "Expression" &&
    thirdToLastS == "{}" &&
    secondToLastS == "Expression" &&
    this->allowsApplyFunctionInPreceding(lastS)
  ) {
    return this->replaceEXEXByEX();
  }
  //end of ambiguity.
  if (
    fifthToLastS == "\\lim" &&
    fourthToLastS == "_" &&
    thirdToLastS == "Expression" &&
    secondToLastS == "Expression" &&
    this->allowsLimitProcessInPreceding(lastS)
  ) {
    return this->replaceOXEEXByEX();
  }
  if (
    fourthToLastS == "Expression" &&
    thirdToLastS == "_" &&
    secondToLastS == "Expression" &&
    lastS != "_"
  ) {
    return this->replaceEOEXByEX();
  }
  if (fourthToLastS == "{"  && thirdToLastS == "{}" && secondToLastS == "Expression" && lastS == "}") {
    return this->replaceXYYXByYY();
  }
  if (secondToLastS == "Expression" && lastS == "!") {
    return this->replaceEOByE();
  }
  if (
    thirdToLastS == "\\binom" &&
    secondToLastS == "Expression" &&
    lastS == "Expression"
  ) {
    return this->replaceXEEBy_OofEE(this->owner->opBinom());
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "^" &&
    fourthToLastS == "Expression" &&
    this->allowsPowerInPreceding(lastS) &&
    this->allowsPowerInNext(fifthToLastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "\\setminus" &&
    fourthToLastS == "Expression" &&
    this->allowsPowerInPreceding(lastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "or" &&
    fourthToLastS == "Expression" &&
    this->allowsOrInPreceding(lastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "and" &&
    fourthToLastS == "Expression" &&
    this->allowsAndInPreceding(lastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "+" &&
    fourthToLastS == "Expression" &&
    this->allowsPlusInPreceding(lastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "\\oplus" &&
    fourthToLastS == "Expression" &&
    this->allowsPlusInPreceding(lastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    fifthToLastS == "|" && secondToLastS == "Expression" &&
    thirdToLastS == "+" && fourthToLastS == "Expression" && lastS == "|"
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    fifthToLastS == "Expression" &&
    fourthToLastS == "^" &&
    thirdToLastS == "-" &&
    secondToLastS == "Expression" && (
      lastS == "Expression" ||
      lastS == "Variable"
  )) {
    return this->replaceEPowerMinusEXByEX();
  }
  if (
    thirdToLastS == "-" &&
    secondToLastS == "Expression" &&
    fourthToLastS == "Expression" &&
    this->allowsPlusInPreceding(lastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    thirdToLastS == "-" &&
    secondToLastS == "Expression" &&
    fourthToLastS == "Expression" &&
    lastS == "|" &&
    fifthToLastS == "|"
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" && thirdToLastS == "-" && this->allowsPlusInPreceding(lastS)
  ) {
    return this->replaceOEXByEX();
  }
  if (secondToLastS == "Expression" && thirdToLastS == "-" && lastS == "|" && fourthToLastS == "|") {
    return this->replaceOEXByEX();
  }
  if (secondToLastS == "Expression" && thirdToLastS == "mod" && fourthToLastS == "Expression") {
    return this->replaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "\\in" &&
    fourthToLastS == "Expression" &&
    this->allowsInInPreceding(lastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (secondToLastS == "Expression" && thirdToLastS == "\\choose" && fourthToLastS == "Expression") {
    return this->replaceEXEXBy_OofEE_X(this->owner->opBinom());
  }
  if (secondToLastS == "Expression" && thirdToLastS == "\\otimes" && fourthToLastS == "Expression" && this->allowsTensorInPreceding(lastS)) {
    return this->replaceEOEXByEX();
  }
  if (secondToLastS == "Expression" && thirdToLastS == "Expression" && fourthToLastS == "\\frac") {
    return this->replaceXEEXBy_OofEE_X(this->owner->opDivide());
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "\\times" &&
    fourthToLastS == "Expression" &&
    this->allowsTimesInPreceding(lastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "*" &&
    fourthToLastS == "Expression" &&
    this->allowsTimesInPreceding(secondToLastE, lastS)) {
    return this->replaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "/" &&
    fourthToLastS == "Expression" &&
    this->allowsDivideInPreceding(lastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "\\to" &&
    fourthToLastS == "Expression" &&
    this->allowsLimitProcessInPreceding(lastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    this->owner->atomsWhoseExponentsAreInterpretedAsFunctions.contains(fifthToLastS) &&
    fourthToLastS == "^" &&
    thirdToLastS == "Expression" &&
    secondToLastS == "Expression" &&
    this->allowsTimesInPreceding(lastS)
  ) {
    return this->replaceOOEEXbyEXpowerLike();
  }

  if (
    fifthToLastS == "LogBase" && fourthToLastS == "_" && thirdToLastS == "Expression" &&
    secondToLastS == "Expression" && this->allowsTimesInPreceding(lastS)
  ) {
    return this->replaceOXEEXByEX();
  }
  if (
    seventhToLastS == "\\sqrt" && sixthToLastS == "[" && fifthToLastS == "Expression" &&
    fourthToLastS == "]" && thirdToLastS == "Expression"
  ) {
    return this->replaceOXEXEXXByEXX();
  }
  if (
    this->allowsTimesInNext(fourthToLastS) &&
    thirdToLastS == "Expression" &&
    secondToLastS == "Expression" &&
    this->allowsTimesInPreceding(secondToLastE, lastS)
  ) {
    this->replaceEEXBy_CofEE_X(this->conTimes());
    Expression impliedFunApplication;
    if (CalculatorBasics::timesToFunctionApplication(
      *this->owner,
      (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2].data,
      impliedFunApplication
    )) {
      (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2].data = impliedFunApplication;
      if (this->flagLogSyntaxRules) {
        this->lastRuleName = "[Rule: implied function application]";
      }
    }
    return true;
  }
  if (
    (lastS == ">" && secondToLastS == "=") ||
    (lastS == "=" && secondToLastS == ">")
  ) {
    return this->replaceXXByCon(this->conGEQ());
  }
  if (thirdToLastS == "|" && secondToLastS == "Expression" && lastS == "|") {
    return this->replaceOEXByE();
  }
  if (thirdToLastS == "(" && secondToLastS == "Expression" && lastS == ")") {
    return this->replaceXEXByE();
  }
  if (thirdToLastS == "{" && secondToLastS == "Expression" && lastS == "}") {
    return this->replaceXEXByE();
  }
  if (fourthToLastS == "{" && thirdToLastS == "Expression" && secondToLastS == "}") {
    return this->replaceXEXYByEY();
  }
  if (thirdToLastS == "(" && secondToLastS == "\\circ" && lastS == ")") {
    return this->replaceOXbyEX();
  }
  if (thirdToLastS == "{" && secondToLastS == "\\circ" && lastS == "}") {
    return this->replaceOXbyEX();
  }
  if (secondToLastS == "^" && lastS == "\\circ") {
    return this->replaceObyE();
  }
  if (this->owner->flagUseBracketsForIntervals) {
    if (thirdToLastS == "(" && secondToLastS == "Expression" && lastS == "]") {
      return this->replaceXEXByEContainingOE(this->owner->opIntervalRightClosed());
    }
    if (thirdToLastS == "[" && secondToLastS == "Expression" && lastS == ")") {
      return this->replaceXEXByEContainingOE(this->owner->opIntervalLeftClosed());
    }
    if (thirdToLastS == "[" && secondToLastS == "Expression" && lastS == "]") {
      return this->replaceXEXByEContainingOE(this->owner->opIntervalClosed());
    }
    if (this->owner->flagUseBracketsForIntervals) {
      if (secondToLastE.data.isSequenceNElements(2)) {
        if (
          this->isSeparatorFromTheLeftForInterval(fourthToLastS) &&
          thirdToLastS == "(" &&
          secondToLastS == "Sequence" &&
          lastS == ")"
        ) {
          return this->replaceXEXByEContainingOE(this->owner->opIntervalOpen());
        }
      }
    }
  }
  if (thirdToLastS == "\"" && secondToLastS == "Expression" && lastS == "\"") {
    return this->replaceXEXByEContainingOE(this->owner->opQuote());
  }
  if (secondToLastS == "\"" && lastS == "\"") {
    return this->replaceXXByEmptyString();
  }
  if (secondToLastS == "==" && lastS == "=") {
    this->replaceXXByCon(this->conEqualEqualEqual());
  }
  if (
    this->isSeparatorFromTheRightGeneral(lastS) &&
    secondToLastS == "Expression" &&
    thirdToLastS == "===" &&
    fourthToLastS == "Expression"
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastS) &&
    this->isSeparatorFromTheRightForDefinition(lastS) &&
    secondToLastS == "Expression" &&
    thirdToLastS == "==" &&
    fourthToLastS == "Expression"
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastS) &&
    this->isSeparatorFromTheRightForDefinition(lastS) &&
    secondToLastS == "Expression" &&
    thirdToLastS == "\\geq" &&
    fourthToLastS == "Expression"
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastS) &&
    this->isSeparatorFromTheRightForDefinition(lastS) &&
    secondToLastS == "Expression" &&
    thirdToLastS == "\\leq" &&
    fourthToLastS == "Expression"
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastS) &&
    this->isSeparatorFromTheRightForDefinition(lastS) &&
    secondToLastS == "Expression" &&
    thirdToLastS == ">" &&
    fourthToLastS == "Expression"
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastS) &&
    this->isSeparatorFromTheRightForDefinition(lastS) &&
    secondToLastS == "Expression" &&
    thirdToLastS == "<" &&
    fourthToLastS == "Expression"
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastS) &&
    fourthToLastS == "Expression" &&
    thirdToLastS == "=" &&
    secondToLastS == "Expression" &&
    this->isSeparatorFromTheRightForDefinition(lastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastS) &&
    fourthToLastS == "Expression" &&
    thirdToLastS == "=:" &&
    secondToLastS == "Expression" &&
    this->isSeparatorFromTheRightForDefinition(lastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    lastS == "Sequence" &&
    lastE.data.size() == 0 &&
    lastE.data.data == this->owner->opList()
  ) {
    return this->replaceXByCon(this->controlSequences.getIndexNoFail("MakeSequence"));
  }
  if (thirdToLastS == "\\sqrt" && secondToLastS == "Expression") {
    return this->replaceSqrtEXByEX();
  }
  if (fourthToLastS == "\\sqrt" && thirdToLastS == "Expression") {
    return this->replaceSqrtEXXByEXX();
  }
  if (fourthToLastS == "\\sqrt" && thirdToLastS == "{}" && secondToLastS == "Expression") {
    return this->replaceSqrtXEXByEX();
  }
  if (
    sixthToLastS == "\\sqrt" && fifthToLastS == "[" && fourthToLastS == "Expression" &&
    thirdToLastS == "]" && secondToLastS == "Expression"
  ) {
    return this->replaceOXEXEXByEX();
  }
  if (
    this->owner->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(thirdToLastS) &&
    secondToLastS == "Expression" && this->allowsTimesInPreceding(lastS)
  ) {
    return this->replaceOEXByEX();
  }
  if (
    this->owner->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(fourthToLastS) &&
    thirdToLastS == "^" && secondToLastS == "Expression" && this->isSeparatorFromTheRightGeneral(lastS)
  ) {
    return this->replaceC1C2Eby_C2ofC1E();
  }
  if (
    this->owner->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(fourthToLastS) &&
    thirdToLastS == "{}" && secondToLastS == "Expression" && this->allowsApplyFunctionInPreceding(lastS)
  ) {
    return this->replaceOXEXByEX();
  }
  if (
    this->owner->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(secondToLastS) &&
    this->isSeparatorFromTheRightGeneral(lastS)
  ) {
    return this->replaceOXbyEX();
  }
  if (fourthToLastS == "\\log" && thirdToLastS == "_" && secondToLastS == "Expression") {
    return this->replaceXYYYByConYYY(this->controlSequences.getIndexNoFail("LogBase"));
  }
  //Some synonyms:
  if (lastS == "ln" || lastS == "log" || lastS == "\\ln") {
    return this->replaceXByCon(this->controlSequences.getIndexNoFail("\\log"));
  }
  if (lastS == "infinity" || lastS == "infty") {
    return this->replaceXByO(this->owner->opInfinity());
  }
  if (lastS == "pi") {
    return this->replaceXByO(this->owner->opPi());
  }
  if (lastS == "arcsin") {
    return this->replaceXByCon(this->controlSequences.getIndexNoFail("\\arcsin"));
  }
  if (lastS == "arccos") {
    return this->replaceXByCon(this->controlSequences.getIndexNoFail("\\arccos"));
  }
  if (lastS == "arctan") {
    return this->replaceXByCon(this->controlSequences.getIndexNoFail("\\arctan"));
  }
  if (lastS == "sin") {
    return this->replaceXByCon(this->controlSequences.getIndexNoFail("\\sin"));
  }
  if (lastS == "cos") {
    return this->replaceXByCon(this->controlSequences.getIndexNoFail("\\cos"));
  }
  if (lastS == "tan") {
    return this->replaceXByCon(this->controlSequences.getIndexNoFail("\\tan"));
  }
  if (lastS == "cot") {
    return this->replaceXByCon(this->controlSequences.getIndexNoFail("\\cot"));
  }
  if (lastS == "csc") {
    return this->replaceXByCon(this->controlSequences.getIndexNoFail("\\csc"));
  }
  if (lastS == "sec") {
    return this->replaceXByCon(this->controlSequences.getIndexNoFail("\\sec"));
  }
  if (lastS == "sqrt") {
    return this->replaceXByCon(this->controlSequences.getIndexNoFail("\\sqrt"));
  }
  //end of some synonyms
  if (fourthToLastS == "Expression" && thirdToLastS == "\\cup" && secondToLastS == "Expression" && this->allowsTimesInPreceding(lastS)) {
    return this->replaceEOEXByEX();
  }
  if (fourthToLastS == "Expression" && thirdToLastS == "\\cap" && secondToLastS == "Expression" && this->allowsTimesInPreceding(lastS)) {
    return this->replaceEOEXByEX();
  }
  if (fourthToLastS == "Expression" && thirdToLastS == "\\sqcup" && secondToLastS == "Expression" && this->allowsTimesInPreceding(lastS)) {
    return this->replaceEOEXByEX();
  }
  if (thirdToLastS == "if" && secondToLastS == "Expression" && this->allowsIfInPreceding(lastS)) {
    return this->replaceOXXByEXX();
  }
  if (
    fourthToLastS == "Sequence" && thirdToLastS == "," &&
    secondToLastS == "Expression" && (
      lastS == "," || lastS == ")" || lastS == "}" ||
      (this->owner->flagUseBracketsForIntervals && lastS == "]")
    )
  ) {
    return this->replaceSequenceUXEYBySequenceZY(this->conSequence());
  }
  if (
    (thirdToLastS != "[" || this->owner->flagUseBracketsForIntervals)&&
    secondToLastS == "Expression" && lastS == ","
  ) {
    return this->replaceYXBySequenceX(this->conSequence());
  }
  if (thirdToLastS == "MakeSequence" && secondToLastS == "{}" && lastS == "Expression") {
    return this->replaceXXYBySequenceY(this->conExpression());
  }
  if (fourthToLastS == "MakeSequence" && thirdToLastS == "{}" && secondToLastS == "Expression") {
    return this->replaceXXYXBySequenceYX(this->conExpression());
  }
  if (
    fifthToLastS == "Expression" && fourthToLastS == "{}" &&
    thirdToLastS == "(" &&
    secondToLastS == "Sequence" && lastS == ")"
  ) {
    return this->replaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  }
  if (
    fifthToLastS == "Expression" && fourthToLastS == "{}" &&
    thirdToLastS == "{" &&
    secondToLastS == "Sequence" && lastS == "}"
  ) {
    return this->replaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  }
  if (
    secondToLastS == "Sequence" && (
      (thirdToLastS == "(" && lastS == ")") ||
      (thirdToLastS == "{" && lastS == "}") || (
        this->owner->flagUseBracketsForIntervals && (
          (thirdToLastS == "[" && lastS == ")") ||
          (thirdToLastS == "(" && lastS == "]") ||
          (thirdToLastS == "[" && lastS == "]")
        )
      )
    )
  ) {
    return this->replaceXXbyEX();
  }
  if (
    fourthToLastS == "\\begin" &&
    thirdToLastS == "{" &&
    secondToLastS == "pmatrix" &&
    lastS == "}"
  ) {
    return this->replaceXdotsXByMatrixStart(4);
  }
  if (
    fifthToLastS == "\\begin" && fourthToLastS == "{" &&
    thirdToLastS == "array" && secondToLastS == "}" && lastS == "Expression"
  ) {
    return this->replaceXdotsXByMatrixStart(5);
  }
  if (
    fourthToLastS == "\\end" && thirdToLastS == "{" &&
    (secondToLastS == "array"  || secondToLastS == "pmatrix") &&
    lastS == "}"
  ) {
    return this->replaceXXXXByCon(this->conMatrixEnd());
  }
  if (lastS == "\\" && secondToLastS == "\\") {
    return this->replaceXXByCon(this->controlSequences.getIndexNoFail("\\\\"));
  }
  if (lastS == "\\displaystyle") {
    return this->popTopSyntacticStack();
  }
  if (
    thirdToLastS == "Matrix" && secondToLastS == "Expression" &&
    lastS == "&"
  ) {
    return this->replaceMatrixEXByMatrix();
  }
  if (
    thirdToLastS == "Matrix" && secondToLastS == "Expression" &&
    lastS == "MatrixEnd"
  ) {
    return this->replaceMatrixEXByMatrixX();
  }
  if (
    thirdToLastS == "Matrix" && secondToLastS == "Expression" &&
    lastS == "\\\\"
  ) {
    return this->replaceMatrixEXByMatrixNewRow();
  }
  if (secondToLastS == "Matrix" && lastS == "MatrixEnd") {
    return this->replaceMatrixXByE();
  }
  if (!this->owner->flagUseBracketsForIntervals) {
    if (
      fifthToLastS == "[" && fourthToLastS == "Expression" &&
      thirdToLastS == "," &&
      secondToLastS == "Expression" && lastS == "]"
    ) {
      return this->replaceXEXEXBy_CofEE(this->conLieBracket());
    }
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(eighthToLastS) &&
    seventhToLastS == "Expression" &&
    sixthToLastS == ":" && fifthToLastS == "if" && fourthToLastS == "Expression" &&
    thirdToLastS == "=" && secondToLastS == "Expression" &&
    this->isSeparatorFromTheRightForDefinition(lastS)
  ) {
    return this->replaceEXXEXEXBy_CofEEE_X(this->conDefineConditional());
  }
  if (thirdToLastS == "SequenceStatements" && secondToLastS == "Expression" && (lastS == ")" || lastS == "}")) {
    return this->replaceEXdotsXbySsXdotsX(1);
  }
  if (secondToLastS == "Expression" && lastS == ";") {
    return this->replaceEXdotsXBySs(1);
  }
  if (secondToLastS == "Expression" && lastS == "EndProgram") {
    return this->replaceEXdotsXbySsXdotsX(1);
  }
  if (
    (thirdToLastS == "(" || thirdToLastS == "{" ) &&
    secondToLastS == "SequenceStatements" &&
    (lastS == ")" || lastS == "}")
  ) {
    return this->replaceXEXByE();
  }
  if (secondToLastS == "SequenceStatements" && lastS == "SequenceStatements") {
    return this->replaceSsSsXdotsXbySsXdotsX(0);
  }
  if (
    thirdToLastS == "SequenceStatements" &&
    secondToLastS == "SequenceStatements" &&
    (lastS == "EndProgram" || lastS == ")" || lastS == "}")
  ) {
    return this->replaceSsSsXdotsXbySsXdotsX(1);
  }
  if (secondToLastS == "SequenceStatements" && lastS == "EndProgram") {
    return this->replaceXXByCon(this->conExpression());
  }
  if (lastS == "EndProgram") {
    return this->decreaseStackSetCharacterRanges(1);
  }
  return false;
}

bool CalculatorParser::isDefiniteIntegral(const std::string& syntacticRole) {
  return
  syntacticRole == "\\int_{*}" ||
  syntacticRole == "\\int^{*}" ||
  syntacticRole == "\\int_{*}^{**}";
}
