// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"

SyntacticElement Calculator::getEmptySyntacticElement() {
  SyntacticElement result;
  result.controlIndex = this->controlSequences.getIndex(" ");
  result.theData.reset(*this);
  return result;
}

std::string SyntacticElement::toStringHumanReadable(Calculator& theBoss, bool includeLispifiedExpressions) const {
  std::string controlString;
  if (this->controlIndex < 0) {
    controlString = "Error: control index is not initialized! This is likely a programming error.";
  } else {
    controlString = theBoss.controlSequences[this->controlIndex];
  }
  bool makeTable = this->controlIndex == theBoss.conExpression() ||
  this->controlIndex == theBoss.conError() || this->controlIndex == theBoss.conSequence() ||
  this->controlIndex == theBoss.conSequenceStatements()|| this->controlIndex == theBoss.conVariable();
  if (!makeTable) {
    return controlString;
  }
  std::stringstream out;
  out << "<table style = \"vertical-align:top;border-spacing= 0px 0px\">";
  out << "<tr><td style = \"text-align:center\">" << this->theData.toString(nullptr) << "</td></tr>";
  out << "<tr><td style = \"color:#AAAAAA\">" << controlString << "</td></tr>";
  if (includeLispifiedExpressions) {
    out << "<tr><td style =\"color:#AAAAAA\">" << this->theData.toStringFull() << "</td></tr>";
  }
  if (this->errorString != "") {
    out << "<tr><td>" << this->errorString << "</td></tr>";
  }
  out << "</table>";
  return out.str();
}

Calculator::EvaluationStatistics::EvaluationStatistics() {
  this->reset();
}

void Calculator::EvaluationStatistics::reset() {
  this->expressionsEvaluated = 0;
  this->maximumCallsBeforeReportGeneration = 5000;
  this->callsSinceReport = 0;
  this->totalSubstitutions = 0;
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
}

void Calculator::reset() {
  this->statistics.reset();
  this->maximumAlgebraicTransformationsPerExpression = 100;
  this->MaxRuleStacksCached = 500;
  this->maximumCachedExpressionPerRuleStack = 100000;
  this->maximumRecursionDepth = 10000;
  this->recursionDepth = 0;

  this->depthRecursionReached = 0;
  this->flagWriteLatexPlots = false;
  this->flagLogSyntaxRules = false;
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
  this->flagNoApproximations = false;
  this->flagDefaultRulesWereTamperedWith = false;
  this->flagUsePredefinedWordSplits = true;
  this->flagPlotNoControls = true;
  this->flagPlotShowJavascriptOnly = false;
  this->flagHasGraphics = false;
  this->flagUseBracketsForIntervals = false;

  this->maximumLatexChars = 2000;
  this->numEmptyTokensStart = 9;
  this->theObjectContainer.reset();
  this->controlSequences.clear();

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
  this->stringsThatSplitIfFollowedByDigit.clear();
  this->knownFunctionsWithComplexRange.clear();
  this->knownDoubleConstants.clear();
  this->knownDoubleConstantValues.setSize(0);
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.clear();

  this->syntacticSoup.setSize(0);
  this->syntacticStacK.setSize(0);
  this->flagTimeLimitErrorDetected = false;
  this->flagFirstErrorEncountered = false;
  this->flagMaxTransformationsErrorEncountered = false;
  this->flagMaxRecursionErrorEncountered = false;
  this->flagAbortComputationASAP = false;
  this->flagDisplayContext = false;
  this->evaluatedExpressionsStack.clear();
  this->theCruncherIds.clear();
  this->theCruncherS.setSize(0);
  this->syntaxErrors = "";
  this->evaluationErrors.setSize(0);
  this->currentSyntacticStack = &this->syntacticStacK;
  this->currrentSyntacticSoup = &this->syntacticSoup;
  this->cachedExpressions.clear();
  this->imagesCachedExpressions.setSize(0);
  this->theProgramExpression.reset(*this);
  this->RuleStackCacheIndex = - 1;
  this->ruleStack.reset(*this,this->MaxRuleStacksCached);
  this->cachedRuleStacks.clear();
  // The expression container must be cleared second to last.
  this->allChildExpressions.clear();
  // The hashes list below is used in computing the hashes of the list above.
  // It must therefore be cleared last.
  this->allChildExpressionHashes.clear();
}

void Calculator::initialize() {
  MacroRegisterFunctionWithName("Calculator::initialize");
  this->reset();

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
  this->addOperationBuiltInType("RationalFunctionModuloInteger");
  this->addOperationBuiltInType("string");
  this->addOperationBuiltInType("ElementUEoverRF");
  this->addOperationBuiltInType("ElementTensorGVM");
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

  // Empty token comes first.
  this->controlSequences.addOnTopNoRepetitionMustBeNew(" ");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("{{}}");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("Variable");
  this->controlSequences.addListOnTop(this->operations.theKeys);//all operations defined up to this point are also control sequences
  this->controlSequences.addOnTopNoRepetitionMustBeNew("Expression");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("Integer");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("{}");
  this->controlSequences.addOnTopNoRepetitionMustBeNew(",");
  this->controlSequences.addOnTopNoRepetitionMustBeNew(".");
  //  this->controlSequences.addOnTopNoRepetitionMustBeNew("\"");
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
  this->controlSequences.addOnTopNoRepetition(
    this->knownOperationsInterpretedAsFunctionsMultiplicatively
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

  this->initializePredefinedStandardOperationsWithoutHandler();
  this->totalPatternMatchesPerformed = 0;
  this->initPredefinedStandardOperations();
  this->initPredefinedInnerFunctions();
  this->initCalculusTestingFunctions();
  this->initPredefinedOperationsComposite();
  this->initializeAtomsThatAllowCommutingOfArguments();
  this->initAtomsThatFreezeArguments();
  this->initializeAtomsNotGoodForChainRule();
  this->initBuiltInAtomsNotInterpretedAsFunctions();
  this->initializeBuiltInAtomsWhosePowersAreInterpretedAsFunctions();
  this->initializeOperationsThatAreKnownFunctions();
  this->initializeAtomsNonCacheable();
  this->initializeArithmeticOperations();
  this->initializePredefinedWordSplits();
  this->initializeStringsThatSplitIfFollowedByDigit();

  this->initializeToStringHandlers();

  this->ruleStack.reset(*this, 100);
  this->ruleStack.addChildAtomOnTop(this->opEndStatement());
  this->cachedRuleStacks.clear();
  this->RuleStackCacheIndex = 0;
  this->cachedRuleStacks.addOnTop(this->ruleStack);
  this->numberOfPredefinedAtoms = this->operations.size(); //<-operations added up to this point are called ``operations''
  this->checkConsistencyAfterInitialization();
}

bool Calculator::checkPredefinedFunctionNameRepetitions() {
  MacroRegisterFunctionWithName("Calculator::checkPredefinedFunctionNameRepetitions");
  HashedList<std::string, MathRoutines::hashString> ruleIds;
  for (int i = 0; i < this->operations.size(); i ++) {
    MemorySaving<Calculator::OperationHandlers>& current = this->operations.theValues[i];
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

bool Calculator::replaceOXEXEXEXByE() {
  SyntacticElement& opElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 8];
  SyntacticElement& leftE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 6];
  SyntacticElement& middleE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& rightE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.theData);
  newExpr.addChildOnTop(middleE.theData);
  newExpr.addChildOnTop(rightE.theData);
  opElt.theData = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(7);
}

bool Calculator::replaceSqrtEXByEX() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& argument = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->opSqrt());
  newExpr.addChildOnTop(this->expressionTwo());
  newExpr.addChildOnTop(argument.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(1);
}

bool Calculator::replaceSqrtEXXByEXX() {
  int lastSize = (*this->currentSyntacticStack).size;
  SyntacticElement& left = (*this->currentSyntacticStack)[lastSize - 4];
  SyntacticElement& argument = (*this->currentSyntacticStack)[lastSize - 3];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->opSqrt());
  newExpr.addChildOnTop(this->expressionTwo());
  newExpr.addChildOnTop(argument.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  (*this->currentSyntacticStack)[lastSize - 3] = (*this->currentSyntacticStack)[lastSize - 2];
  (*this->currentSyntacticStack)[lastSize - 2] = (*this->currentSyntacticStack)[lastSize - 1];
  (*this->currentSyntacticStack).setSize(lastSize - 1);
  return true;
}

bool Calculator::replaceSqrtXEXByEX() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& argument = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->opSqrt());
  newExpr.addChildOnTop(this->expressionTwo());
  newExpr.addChildOnTop(argument.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(2);
}

bool Calculator::replaceOXEXEByE() {
  SyntacticElement& opElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  SyntacticElement& leftE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& rightE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.theData);
  newExpr.addChildOnTop(rightE.theData);
  opElt.theData = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(4);
}

bool Calculator::replaceOXEXEXByEX() {
  SyntacticElement& opElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 6];
  SyntacticElement& leftE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& rightE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.theData);
  newExpr.addChildOnTop(rightE.theData);
  opElt.theData = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(4);
}

bool Calculator::replaceOXEXEXXByEXX() {
  SyntacticElement& opElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 7];
  SyntacticElement& leftE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  SyntacticElement& rightE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.theData);
  newExpr.addChildOnTop(rightE.theData);
  opElt.theData = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->decreaseStackExceptLastTwo(4);
}

void Calculator::logPublicError(const std::string& theError) {
  if (this->errorsPublic.str() != "") {
    this->errorsPublic << theError;
  }
}

bool Calculator::decreaseStackExceptLast(int decrease) {
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

bool Calculator::decreaseStackExceptLastTwo(int decrease) {
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

bool Calculator::replaceOXEEXByEX() {
  SyntacticElement& opElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  SyntacticElement& leftE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& rightE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr(*this);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.theData);
  newExpr.addChildOnTop(rightE.theData);
  opElt.theData = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(3);
}

bool Calculator::replaceOXXEXEXEXByE() {
  SyntacticElement& opElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 9];
  SyntacticElement& leftE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 6];
  SyntacticElement& middleE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& rightE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.theData);
  newExpr.addChildOnTop(middleE.theData);
  newExpr.addChildOnTop(rightE.theData);
  opElt.theData = newExpr;
  opElt.controlIndex = this->conExpression();
  this->decreaseStackSetCharacterRanges(8);
  return true;
}

bool Calculator::replaceOXEXByEX() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(left.controlIndex));
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(2);
}

bool Calculator::replaceOXXByEXX() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceOXXByEXX]";
  }
  left.theData.makeAtom(this->getOperationIndexFromControlIndex(left.controlIndex), *this);
  left.controlIndex = this->conExpression();
  return true;
}

bool Calculator::replaceOXEByE() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(left.controlIndex));
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(2);
}

bool Calculator::replaceOEByE() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(left.controlIndex));
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(1);
}

bool Calculator::replaceOEXByEX() {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceOEXByEX]";
  }
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.addChildOnTop(right.theData);
  middle.theData = newExpr;
  middle.controlIndex = this->conExpression();
  this->decreaseStackExceptLast(1);
  return true;
}

bool Calculator::replaceXXXByCon(int theCon) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3].controlIndex = theCon;
  this->decreaseStackSetCharacterRanges(2);
  return true;
}

bool Calculator::replaceXXXByConCon(int con1, int con2) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2].controlIndex = con1;
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3].controlIndex = con2;
  this->decreaseStackSetCharacterRanges(1);
  return true;
}

bool Calculator::replaceXXXXXByCon(int theCon) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5].controlIndex = theCon;
  this->decreaseStackSetCharacterRanges(4);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceXXXXXByCon]";
  }
  return true;
}

bool Calculator::replaceXXXXXByConCon(int con1, int con2) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4].controlIndex = con2;
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5].controlIndex = con1;
  this->decreaseStackSetCharacterRanges(3);
  return true;
}

bool Calculator::replaceXXXXByConCon(int con1, int con2) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3].controlIndex = con2;
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4].controlIndex = con1;
  this->decreaseStackSetCharacterRanges(2);
  return true;
}

bool Calculator::replaceXXXXByCon(int con1) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4].controlIndex = con1;
  this->decreaseStackSetCharacterRanges(3);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceXXXXByCon]";
  }
  return true;
}

bool Calculator::replaceXByCon(int theCon) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1].controlIndex = theCon;
  return true;
}

bool Calculator::replaceXByO(int theOperation) {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1].controlIndex = this->conExpression();
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1].theData.makeAtom(theOperation, *this);
  return true;
}

bool Calculator::replaceXByConCon(int con1, int con2) {
  (*this->currentSyntacticStack).setSize((*this->currentSyntacticStack).size + 1);
  (*this->currentSyntacticStack).lastObject()->theData.reset(*this);
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2].controlIndex = con1;
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1].controlIndex = con2;
  return true;
}

bool Calculator::replaceOEXByE() {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceOEXByE]";
  }
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.addChildOnTop(right.theData);
  middle.theData = newExpr;
  middle.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(2);
}

bool Calculator::replaceXXByEmptyString() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  left.theData.assignValue(std::string(""), *this);
  left.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(1);
}

bool Calculator::replaceEOByE() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(right.controlIndex));
  newExpr.addChildOnTop(left.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  this->decreaseStackSetCharacterRanges(1);
  return true;
}

bool Calculator::isRightSeparator(unsigned char c) {
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

bool Calculator::isLeftSeparator(unsigned char c) {
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

bool Calculator::isInterpretedAsEmptySpace(const std::string& input) {
  if (input.size() != 1) {
    return false;
  }
  return this->isInterpretedAsEmptySpace(static_cast<unsigned char>(input[0]));
}

bool Calculator::isInterpretedAsEmptySpace(unsigned char input) {
  switch (input) {
   
  case '\n': //numerical value: 10
  case '~':
  case '\r': //numerical value: 13
  case '\t':
  case 160: //&nbsp
    return true;
  default: return false;
  }
  // return false;
}

void Calculator::parseFillDictionary(const std::string& input) {
  MacroRegisterFunctionWithName("Calculator::parseFillDictionary");
  this->parseFillDictionary(input, *this->currrentSyntacticSoup);
  SyntacticElement currentElement;
  currentElement.theData.reset(*this);
  currentElement.controlIndex = this->conEndProgram();
  (*this->currrentSyntacticSoup).addOnTop(currentElement);
}

bool Calculator::shouldSplitOutsideQuotes(const std::string& left, char right) {
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

void Calculator::parseFillDictionary(const std::string& input, List<SyntacticElement>& output) {
  MacroRegisterFunctionWithName("Calculator::parseFillDictionary");
  std::string current;
  output.reserve(static_cast<signed>(input.size()));
  output.setSize(0);
  char LookAheadChar;
  SyntacticElement currentElement;
  bool inQuotes = false;
  bool escapedInQuotes = false;
  bool escapingAllowed = true;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (escapedInQuotes) {
      if (current.size() > 0) {
        current.resize(current.size() - 1);
      }
      escapedInQuotes = false;
      escapingAllowed = false;
    }
    current.push_back(input[i]);
    if (i + 1 < input.size()) {
      LookAheadChar = input[i + 1];
    } else {
      LookAheadChar = ' ';
      if (inQuotes) {
        LookAheadChar = '"';
      }
    }
    if (!inQuotes) {
      if (this->isInterpretedAsEmptySpace(current)) {
        current = " ";
      }
    }
    bool shouldSplit = false;
    if (!inQuotes) {
      shouldSplit = this->shouldSplitOutsideQuotes(current, LookAheadChar);
    } else {
      if (escapingAllowed) {
        if (current.size() >= 1) {
          if (current[current.size() - 1] == '\\') {
            if (LookAheadChar == '\\' || LookAheadChar == '"') {
              escapedInQuotes = true;
              continue;
            }
          }
        }
      }
      shouldSplit = (LookAheadChar == '"');
    }
    if (current == "\"" && escapingAllowed) {
      inQuotes =!inQuotes;
      shouldSplit = true;
    }
    if (!shouldSplit) {
      escapingAllowed = true;
      continue;
    }
    bool mustInterpretAsVariable = false;
    if (inQuotes && (current != "\"" || !escapingAllowed)) {
      mustInterpretAsVariable = true;
    }
    escapingAllowed = true;
    if (this->controlSequences.contains(current) && !mustInterpretAsVariable) {
      currentElement.controlIndex = this->controlSequences.getIndex(current);
      currentElement.theData.reset(*this);
      output.addOnTop(currentElement);
    } else if (MathRoutines::hasDecimalDigitsOnly(current) && !mustInterpretAsVariable) {
      currentElement.theData.assignValue(current, *this);
      currentElement.controlIndex = this->conInteger();
      output.addOnTop(currentElement);
    } else {
      currentElement.controlIndex = this->controlSequences.getIndex("Variable");
      currentElement.theData.makeAtom(this->addOperationNoRepetitionOrReturnIndexFirst(current), *this);
      output.addOnTop(currentElement);
    }
    current = "";
  }
}

int Calculator::getOperationIndexFromControlIndex(int controlIndex) {
  return this->operations.getIndex(this->controlSequences[controlIndex]);
}

int Calculator::getExpressionIndex() {
  return this->controlSequences.getIndex("Expression");
}

bool Calculator::replaceXXbyE() {
  this->replaceXXbyEX();
  this->decreaseStackSetCharacterRanges(1);
  return true;
}

bool Calculator::replaceXXXbyE() {
  this->replaceXXbyEX();
  return this->replaceXEXByE();
}

bool Calculator::replaceOXdotsXbyEXdotsX(int numXs) {
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1 - numXs];
  theElt.theData.makeAtom(this->getOperationIndexFromControlIndex(theElt.controlIndex), *this);
  if (this->flagLogSyntaxRules) {
    std::stringstream out;
    out << "[Rule: Calculator::replaceOXdotsXbyEXdotsX: " << numXs << "]";
    this->parsingLog += out.str();
  }
  theElt.controlIndex = this->conExpression();
  return true;
}

bool Calculator::replaceOXbyEX() {
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  theElt.theData.makeAtom(this->getOperationIndexFromControlIndex(theElt.controlIndex), *this);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceOXbyEX]";
  }
  return this->replaceXXbyEX();
}

bool Calculator::replaceObyE() {
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  theElt.theData.makeAtom(this->getOperationIndexFromControlIndex(theElt.controlIndex), *this);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceObyE]";
  }
  theElt.controlIndex = this->conExpression();
  return true;
}

bool Calculator::replaceXXbyEX() {
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  theElt.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceXXbyEX]";
  }
  return true;
}

bool Calculator::replaceEXXSequenceXBy_Expression_with_E_instead_of_sequence() {
  SyntacticElement& theSequenceElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& theFunctionElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.reserve(theSequenceElt.theData.children.size);
  newExpr.addChildOnTop(theFunctionElt.theData);
  if (theSequenceElt.theData.isAtom()) {
    newExpr.addChildOnTop(theSequenceElt.theData);
  } else {
    for (int i = 1; i < theSequenceElt.theData.children.size; i ++) {
      newExpr.addChildOnTop(theSequenceElt.theData[i]);
    }
  }
  theFunctionElt.theData = newExpr;
  return this->decreaseStackSetCharacterRanges(4);
}

bool Calculator::replaceXEXByE() {
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3] =
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3].controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(2);
}

bool Calculator::replaceXEXByEContainingOE(int inputOpIndex) {
  SyntacticElement& outputElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& inputElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceXEXByEContainingOE]";
  }
  outputElt.theData.reset(*this, 2);
  outputElt.theData.addChildAtomOnTop(inputOpIndex);
  outputElt.theData.addChildOnTop(inputElt.theData);
  outputElt.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(2);
}

bool Calculator::replaceXXByEEmptySequence() {
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  theElt.theData.reset(*this, 1);
  theElt.theData.addChildAtomOnTop(this->opSequence());
  theElt.controlIndex = this->conExpression();
  return this->decreaseStackSetCharacterRanges(1);
}

bool Calculator::isBoundVariableInContext(int inputOp) {
  return this->boundVariablesInContext.contains(inputOp);
}

bool Calculator::isNonBoundVariableInContext(int inputOp) {
  return this->nonBoundVariablesInContext.contains(inputOp);
}

bool Calculator::replaceXXVXdotsXbyE_BOUND_XdotsX(int numXs) {
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - numXs - 1];
  int theBoundVar = theElt.theData.theData;
  if (this->isNonBoundVariableInContext(theBoundVar)) {
    std::stringstream out;
    out << "Syntax error. In the same syntactic scope, the string "
    << this->operations.theKeys[theBoundVar]
    << " is first used to denote a non-bound variable "
    << "but later to denote a bound variable. This is not allowed. ";
    theElt.errorString = out.str();
    theElt.controlIndex = this->conError();
    this->decreaseStackSetCharacterRanges(numXs);
    this->replaceXXYByY();
    return true;
  }
  if (!this->isBoundVariableInContext(theBoundVar)) {
    this->boundVariablesInContext.addOnTopNoRepetition(theBoundVar);
  }
  theElt.theData.reset(*this, 2);
  theElt.theData.addChildAtomOnTop(this->opBind());
  theElt.theData.addChildAtomOnTop(theBoundVar);
  theElt.controlIndex = this->conExpression();
  this->decreaseStackSetCharacterRanges(numXs);
  this->replaceXXYByY();
  return true;
}

bool Calculator::replaceVXdotsXbyE_NONBOUND_XdotsX(int numXs) {
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1 - numXs];
  int theBoundVar = theElt.theData.theData;
  if (this->isBoundVariableInContext(theBoundVar)) {
    theElt.theData.reset(*this, 2);
    theElt.theData.addChildAtomOnTop(this->opBind());
    theElt.theData.addChildAtomOnTop(theBoundVar);
  } else {
    theElt.theData.makeAtom(theBoundVar, *this);
    if (!this->isNonBoundVariableInContext(theBoundVar)) {
      this->nonBoundVariablesInContext.addOnTop(theBoundVar);
    }
  }
  theElt.controlIndex = this->conExpression();
  return true;
}

bool Calculator::replaceOOEEXbyEXpowerLike() {
  SyntacticElement& outerO = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& innerO = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  SyntacticElement& innerArg = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& outerArg = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceOOEEXbyEXpowerLike]";
  }
  Expression newInnerE(*this), newFinalE(*this);
  newInnerE.addChildAtomOnTop(this->getOperationIndexFromControlIndex(innerO.controlIndex));
  newInnerE.addChildOnTop(innerArg.theData);
  newFinalE.addChildAtomOnTop(this->getOperationIndexFromControlIndex(outerO.controlIndex));
  newFinalE.addChildOnTop(newInnerE);
  newFinalE.addChildOnTop(outerArg.theData);

  innerO.theData = newFinalE;
  innerO.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(3);
}

bool Calculator::replaceCXByE() {
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  theElt.controlIndex = this->conExpression();
  return this->popTopSyntacticStack();
}

bool Calculator::replaceCXByEX() {
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  theElt.controlIndex = this->conExpression();
  return true;
}

bool Calculator::replaceXdotsXByMatrixStart(int numXes) {
  SyntacticElement& currentElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - numXes];
  currentElt.dataList.setExpectedSize(10);
  currentElt.dataList.setSize(1);
  currentElt.dataList.lastObject()->makeSequence(*this);
  currentElt.controlIndex = this->conMatrixStart();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceXdotsXByMatrixStart]";
  }
  return this->decreaseStackSetCharacterRanges(numXes - 1);
}

bool Calculator::replaceMatrixXByE() {
  SyntacticElement& theMatElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Matrix<Expression> theMat;
  int numRows = theMatElt.dataList.size;
  int numCols = 0;
  for (int i = 0; i < theMatElt.dataList.size; i ++) {
    numCols = MathRoutines::maximum(theMatElt.dataList[i].size() - 1, numCols);
  }
  if (numCols > 0 && numRows > 0) {
    theMat.initialize(numRows, numCols);
    for (int i = 0; i < numRows; i ++) {
      for (int j = 0; j < numCols; j ++) {
        if (j + 1 >= theMatElt.dataList[i].size()) {
          theMat.elements[i][j].assignValue(0, *this);
          continue;
        }
        theMat.elements[i][j] = theMatElt.dataList[i][j + 1];
      }
    }
    theMatElt.theData.assignMatrixExpressions(theMat, *this, true, true);
  } else {
    theMatElt.theData.makeMatrix(*this);
  }
  theMatElt.dataList.setSize(0);
  theMatElt.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceMatrixXByE]";
  }
  return this->decreaseStackSetCharacterRanges(1);
}

bool Calculator::replaceMatrixEXByMatrixNewRow() {
  SyntacticElement& theMatElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  theMatElt.dataList.lastObject()->addChildOnTop(theElt.theData);
  theMatElt.dataList.setSize(theMatElt.dataList.size + 1);
  theMatElt.dataList.lastObject()->makeSequence(*this);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceMatrixEXByMatrixNewRow]";
  }
  return this->decreaseStackSetCharacterRanges(2);
}

bool Calculator::replaceMatrixEXByMatrix() {
  SyntacticElement& theMatElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  theMatElt.dataList.lastObject()->addChildOnTop(theElt.theData);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceMatrixEXByMatrix]";
  }
  return this->decreaseStackSetCharacterRanges(2);
}

bool Calculator::replaceMatrixEXByMatrixX() {
  SyntacticElement& theMatElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  theMatElt.dataList.lastObject()->addChildOnTop(theElt.theData);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceMatrixEXByMatrixX]";
  }
  return this->decreaseStackExceptLast(1);
}

bool Calculator::replaceAXbyEX() {
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  theElt.controlIndex = this->conExpression();
  return true;
}

std::string SyntacticElement::getIntegerStringCrashIfNot(Calculator& owner) {
  if (this->controlIndex != owner.conInteger()) {
    global.fatal << "Request to get rational from a non-rational element. " << global.fatal;
  }
  std::string result = this->theData.getValue<std::string>();
  for (unsigned i = 0; i < result.size(); i ++) {
    if (!MathRoutines::isADigit(result[i])) {
      global.fatal << "Integer string non-digit entries. " << global.fatal;
    }
  }
  return result;
}

bool Calculator::replaceIntegerDotIntegerByE() {
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
  left.theData.assignValue(result, *this);
  this->decreaseStackSetCharacterRanges(2);
  return true;
}

bool Calculator::replaceIntegerXbyEX() {
  SyntacticElement& theElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  theElt.controlIndex = this->conExpression();
  Rational value;
  value.assignString(theElt.theData.getValue<std::string>());
  theElt.theData.assignValue(value, *this);
  return true;
}

std::string Calculator::toStringIsCorrectAsciiCalculatorString(const std::string& input) {
  std::stringstream out;
  HashedList<char, MathRoutines::hashChar> theBadChars;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (!this->isStandardCalculatorCharacter(static_cast<unsigned char>(input[i]))) {
      theBadChars.addOnTopNoRepetition(input[i]);
    }
  }
  if (theBadChars.size > 0) {
    out << "Non-ascii characters detected in your input, namely: "
    << theBadChars.toStringCommaDelimited() << ", ";
    List<int> ListInt;
    ListInt = theBadChars;
    out << "with respective code numbers: " << ListInt.toStringCommaDelimited() << ". ";
    out << "Perhaps you copy+pasted from webpage/pdf file or are using non-English keyboard setup? ";
  }
  return out.str();
}

bool Calculator::isStandardCalculatorCharacter(unsigned char input) {
  if (10 <= input && input <= 126) {
    return true;
  }
  if (this->isInterpretedAsEmptySpace(input)) {
    return true;
  }
  return false;
}

bool Calculator::allowsApplyFunctionInPreceding(const std::string& lookAhead) {
  return
  lookAhead != "{" && lookAhead != "_" &&
  lookAhead != "\\circ" && lookAhead != "{}" &&
  lookAhead != "$";
}

bool Calculator::replaceSequenceUXEYBySequenceZY(int theControlIndex) {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& afterleft = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  left.theData.addChildOnTop(right.theData);
  left.controlIndex = theControlIndex;
  afterleft = *this->currentSyntacticStack->lastObject();
  this->decreaseStackExceptLast(2);
  return true;
}

bool Calculator::replaceYXBySequenceX(int theControlIndex) {
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceYXBySequenceX]";
  }
  return this->replaceYXdotsXBySequenceYXdotsX(theControlIndex, 1);
}

bool Calculator::replaceYBySequenceY(int theControlIndex) {
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceYBySequenceY]";
  }
  return this->replaceYXdotsXBySequenceYXdotsX(theControlIndex, 0);
}

bool Calculator::replaceXXYBySequenceY(int theControlIndex) {
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceXXYBySequenceY]";
  }
  this->replaceYBySequenceY(theControlIndex);
  return this->replaceXXYByY();
}

bool Calculator::replaceXXYXBySequenceYX(int theControlIndex) {
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceXXYXBySequenceYX]";
  }
  this->replaceYXdotsXBySequenceYXdotsX(theControlIndex, 1);
  return this->replaceXXYXByYX();
}

bool Calculator::replaceSequenceXEBySequence(int theControlIndex) {
  SyntacticElement& left =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  left.theData.addChildOnTop(right.theData);
  left.controlIndex = theControlIndex;
  this->decreaseStackSetCharacterRanges(2);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceSequenceXEBySequence]";
  }
  return true;
}

bool Calculator::replaceYXdotsXBySequenceYXdotsX(int theControlIndex, int numXs) {
  SyntacticElement& main = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - numXs - 1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->opSequence());
  newExpr.addChildOnTop(main.theData);
  main.theData = newExpr;
  main.controlIndex = theControlIndex;
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceYXdotsXBySequenceYXdotsX]";
  }
  return true;
}

bool Calculator::replaceEXEBySequence(int theControlIndex) {
  SyntacticElement& left =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->opSequence());
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = theControlIndex;
  this->decreaseStackSetCharacterRanges(2);
  return true;
}

bool Calculator::replaceEEBy_CofEE(int theControlIndex) {
  SyntacticElement& left =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(theControlIndex));
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  this->decreaseStackSetCharacterRanges(1);
  return true;
}

bool Calculator::replaceEOXbyEX() {
  SyntacticElement& left =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& opElt = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  int theOp = this->getOperationIndexFromControlIndex(opElt.controlIndex);
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(theOp);
  newExpr.addChildOnTop(left.theData);
  left.theData = newExpr;
  this->decreaseStackExceptLast(1);
  return true;
}

bool Calculator::replaceVbyVdotsVAccordingToPredefinedWordSplits() {
  MacroRegisterFunctionWithName("Calculator::replaceVbyVdotsVAccordingToPredefinedWordSplits");
  SyntacticElement& theE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  const std::string& currentVar = this->operations.theKeys[theE.theData.theData];
  if (!this->predefinedWordSplits.contains(currentVar)) {
    global.fatal << "Predefined word splits array does not contain the variable: " << theE.theData.toString()
    << ". This should not happen in the body of this function. " << global.fatal;
  }
  List<std::string>& theSplit = this->predefinedWordSplits.getValueCreate(currentVar);
  SyntacticElement newElt;
  this->popTopSyntacticStack();
  *this << "Predefined symbol replacement: replacing "
  << currentVar << " with the sequence of symbols " << theSplit.toStringCommaDelimited()
  << ". If you do not want such replacements to take "
  << "place you should add the %DontUsePredefinedWordSplits option "
  << "at the start of your input. "
  << "The predefined symbol replacements are used to guard "
  << "the user from accidental typos such as confusing "
  << "x y (the product of x and y) with xy "
  << "(a single variable whose name contains the letters x and y). "
  ;
  for (int i = 0; i < theSplit.size; i ++) {
    newElt.theData.makeAtom(this->addOperationNoRepetitionOrReturnIndexFirst(theSplit[i]), *this);
    newElt.controlIndex = this->controlSequences.getIndex(theSplit[i]);
    if (newElt.controlIndex == - 1) {
      newElt.controlIndex = this->conVariable();
    }
    (*this->currentSyntacticStack).addOnTop(newElt);
  }
  return true;
}

bool Calculator::replaceEEXBy_CofEE_X(int theControlIndex) {
  SyntacticElement& left =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceEEXBy_CofEE_X]";
  }
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(theControlIndex));
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  this->decreaseStackExceptLast(1);
  return true;
}

bool Calculator::replaceEXXEXEBy_CofEEE(int theControlIndex) {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 6];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(theControlIndex));
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(middle.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  this->decreaseStackSetCharacterRanges(5);
  return true;
}

bool Calculator::replaceEXXEXEXBy_CofEEE_X(int theControlIndex) {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 7];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(theControlIndex));
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(middle.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  this->decreaseStackExceptLast(5);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceEXXEXEXBy_CofEEE_X]";
  }
  return true;
}

bool Calculator::replaceUnderscoreEPowerEbyLimits() {
  SyntacticElement& bottom =
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& top =
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->opLimitBoundary());
  newExpr.addChildOnTop(bottom.theData);
  newExpr.addChildOnTop(top.theData);
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4].theData = newExpr;
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4].controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceUnderscoreEPowerEbyLimits]";
  }
  return this->decreaseStackSetCharacterRanges(3);
}

bool Calculator::replacePowerEUnderScoreEbyLimits() {
  SyntacticElement& bottom =
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& top =
  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  MathRoutines::swap(bottom, top);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replacePowerEUnderScoreEbyLimits]";
  }
  return this->replaceUnderscoreEPowerEbyLimits();
}

bool Calculator::replaceXEXEXBy_CofEE(int theControlIndex) {
  SyntacticElement& lefT = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& result = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 5];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(theControlIndex));
  newExpr.addChildOnTop(lefT.theData);
  newExpr.addChildOnTop(right.theData);
  result.theData = newExpr;
  result.controlIndex = this->conExpression();
  this->decreaseStackSetCharacterRanges(4);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceXEXEXBy_CofEE]";
  }
  return true;
}

bool Calculator::replaceEXdotsXbySsXdotsX(int numDots) {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - numDots - 1];
  bool found = false;
  for (int i = (*this->currentSyntacticStack).size - numDots - 2; i >= 0; i --) {
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
  newExpr.reset(*this);
  newExpr.children.reserve(2);
  newExpr.addChildAtomOnTop(this->opEndStatement());
  newExpr.addChildOnTop(left.theData);
  left.theData = newExpr;
  left.controlIndex = this->conSequenceStatements();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceEXdotsXbySsXdotsX]";
  }
  return true;
}

bool Calculator::replaceSsSsXdotsXbySsXdotsX(int numDots) {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - numDots - 2];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - numDots - 1];
  if (!left.theData.startsWith(this->opEndStatement())) {
    global.fatal << "This is a programming error: replaceSsSsXdotsXbySsXdotsX "
    << "called but left expression is not EndStatement." << global.fatal;
  }
  left.theData.children.reserve(left.theData.children.size + right.theData.children.size - 1);
  for (int i = 1; i < right.theData.children.size; i ++) {
    left.theData.addChildOnTop(right.theData[i]);
  }
  left.controlIndex = this->conSequenceStatements();
  (*this->currentSyntacticStack).popIndexShiftDown((*this->currentSyntacticStack).size - numDots - 1);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceSsSsXdotsXbySsXdotsX]";
  }
  return true;
}

bool Calculator::replaceEXEByCofEE(int theControlIndex) {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(theControlIndex));
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  this->decreaseStackSetCharacterRanges(2);
  return true;
}

bool Calculator::replaceEXEXByEX() {
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  this->decreaseStackExceptLast(2);
  return true;
}

bool Calculator::replaceXXbyO(int theOperation) {
  SyntacticElement& result = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  result.theData.makeAtom(theOperation, *this);
  result.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceXXbyO]";
  }
  return this->decreaseStackSetCharacterRanges(1);
}

bool Calculator::replaceXXYbyOY(int theOperation) {
  SyntacticElement& result = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  result.theData.makeAtom(theOperation, *this);
  result.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceXXYbyOY]";
  }
  return this->decreaseStackExceptLast(1);
}


bool Calculator::replaceEOEXByEX() {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceEOEXByEX]";
  }
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  middle = *(*this->currentSyntacticStack).lastObject();
  this->decreaseStackExceptLast(2);
  return true;
}

bool Calculator::replaceEXEXBy_OofEE_X(int theOp) {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceEOEXByEX]";
  }
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(theOp);
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  middle = *(*this->currentSyntacticStack).lastObject();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceEXEXBy_OofEE_X]";
  }
  return this->decreaseStackExceptLast(2);
}

bool Calculator::replaceC1C2Eby_C2ofC1E() {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& left = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceC1C2Eby_C2ofC1E]";
  }
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.addChildAtomOnTop(this->getOperationIndexFromControlIndex(left.controlIndex));
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  return this->decreaseStackExceptLast(2);
}

bool Calculator::replaceXEEXBy_OofEE_X(int inputOperation) {
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  SyntacticElement& left  =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 4];
  SyntacticElement& right =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(inputOperation);
  newExpr.addChildOnTop(middle.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  middle = *(*this->currentSyntacticStack).lastObject();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceXEEXBy_OofEE_X]";
  }
  return this->decreaseStackExceptLast(2);
}

bool Calculator::replaceXEEBy_OofEE(int inputOperation) {
  SyntacticElement& right =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  SyntacticElement& middle = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  SyntacticElement& left  =  (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 3];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(inputOperation);
  newExpr.addChildOnTop(middle.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::replaceXEEBy_OofEE]";
  }
  return this->decreaseStackSetCharacterRanges(2);
}

bool Calculator::isSeparatorFromTheLeftGeneral(const std::string& input) {
  return input == "{" || input == "(" || input == "[" ||
  input == "," || input == ":" || input == ";" ||
  input == " " || input == "Matrix" ||
  input == "&";
}


bool Calculator::isSeparatorFromTheLeftForInterval(const std::string& input) {
  return input == "," || input == "(" || input == "[" || input == ":" || input == ";" ||
  input == "=" || input == "==" || input == "\\\\" || input == "\\cup" || input == "\\cap" ||
  input == "\\end" || input == "&" || input == "EndProgram";
}

bool Calculator::isSeparatorFromTheRightGeneral(const std::string& input) {
  return input == "}" || input == ")" || input == "]" || input == "," || input == ":" || input == ";" ||
  input == "Matrix" ||
  input == "=" || input == "\\\\" ||
  input == "\\end" || input == "&" || input == "EndProgram";
}

bool Calculator::isSeparatorFromTheLeftForList(const std::string& input) {
  return input == "{" || input == "(" || input == "[" || input == ":" || input == ";" || input == " " ||
  input == "Matrix";
}

bool Calculator::isSeparatorFromTheRightForList(const std::string& input) {
  return input == "}" || input == ")" || input == "]" || input == ":" || input == ";" ||
  input == "Matrix" || input == "\\\\" || input == "\\end";
}

bool Calculator::isSeparatorFromTheLeftForDefinition(const std::string& input) {
  return input == "{" || input == "(" || input == ";" || input == "," ||
  input == "SequenceStatements" || input == " " ||
  input == "or" ||
  input == "and";
}

bool Calculator::isSeparatorFromTheRightForDefinition(const std::string& input) {
  return input == "}" || input == ")" || input == ";" || input == "," ||
  input == "EndProgram" || input == "or" ||
  input == "and"
  ;
}

bool Calculator::allowsTensorInPreceding(const std::string& lookAhead) {
  return this->allowsTimesInPreceding(lookAhead) || lookAhead == "\\otimes";
}

bool Calculator::allowsTimesInNext(const std::string& thePreceding) {
  if (
    thePreceding == "{}" || thePreceding == "^" || thePreceding == "\\sqrt" || thePreceding == "_"
  ) {
    return false;
  }
  return true;
}

bool Calculator::allowsTimesInPreceding(const SyntacticElement& thePreceding, const std::string& lookAhead) {
  if (thePreceding.controlIndex == this->conExpression()) {
    if (thePreceding.theData.isBuiltInAtom()) {
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

bool Calculator::allowsTimesInPreceding(const std::string& lookAhead) {
  return lookAhead == "+" || lookAhead == "-" || lookAhead == "*" || lookAhead == "/" ||
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(lookAhead) ||
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

bool Calculator::allowsPowerInPreceding(const std::string& lookAhead) {
  return lookAhead != "{}";
}

bool Calculator::allowsPowerInNext(const std::string& lookBehind) {
  return lookBehind != "_";
}

bool Calculator::allowsLimitProcessInPreceding(const std::string& lookAhead) {
  return lookAhead == "and" || lookAhead == "or" ||
  lookAhead == "(" || lookAhead == "[" ||
  lookAhead == ")" || lookAhead == "]" || lookAhead == "}" ||
  lookAhead == "," || lookAhead == ";" ||
  lookAhead == ":" || lookAhead == "&" || lookAhead == "Matrix" ||
  lookAhead == "\\" ||
  lookAhead == "EndProgram";
}

bool Calculator::allowsAndInPreceding(const std::string& lookAhead) {
  return lookAhead == "and" || lookAhead == "or" ||
  lookAhead == "(" || lookAhead == "[" ||
  lookAhead == ")" || lookAhead == "]" || lookAhead == "}" ||
  lookAhead == "," || lookAhead == ";" ||
  lookAhead == ":" || lookAhead == "&" || lookAhead == "Matrix" ||
  lookAhead == "\\" ||
  lookAhead == "EndProgram"
  ;
}

bool Calculator::allowsOrInPreceding(const std::string& lookAhead) {
  return lookAhead == "and" || lookAhead == "or" ||
  lookAhead == "(" || lookAhead == "[" ||
  lookAhead == ")" || lookAhead == "]" || lookAhead == "}" ||
  lookAhead == "," || lookAhead == ";" ||
  lookAhead == ":" || lookAhead == "&" || lookAhead == "Matrix" ||
  lookAhead == "\\" ||
  lookAhead == "EndProgram"
  ;
}

bool Calculator::allowsInInPreceding(const std::string& lookAhead) {
  return this->allowsIfInPreceding(lookAhead);
}

bool Calculator::allowsIfInPreceding(const std::string& lookAhead) {
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

bool Calculator::allowsPlusInPreceding(const std::string& lookAhead) {
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

bool Calculator::allowsDivideInPreceding(const std::string& lookAhead) {
  return this->allowsTimesInPreceding(lookAhead);
}

bool Calculator::extractExpressions(Expression& outputExpression, std::string* outputErrors) {
  MacroRegisterFunctionWithName("Calculator::extractExpressions");
  //std::string lookAheadToken;
  std::stringstream errorLog;
  (*this->currentSyntacticStack).reserve((*this->currrentSyntacticSoup).size + this->numEmptyTokensStart);
  (*this->currentSyntacticStack).setSize(this->numEmptyTokensStart);
  for (int i = 0; i < this->numEmptyTokensStart; i ++) {
    (*this->currentSyntacticStack)[i] = this->getEmptySyntacticElement();
  }
  this->parsingLog = "";
  this->nonBoundVariablesInContext.clear();
  this->boundVariablesInContext.clear();
  const int maxNumTimesOneRuleCanBeCalled = 1000;
  int counterReport = 0;
  int symbolsToIssueReport = 100;
  int minMillisecondsPerReport = 200;
  int64_t lastMilliseconds = global.getElapsedMilliseconds();
  ProgressReport theReport;
  for (
    this->counterInSyntacticSoup = 0;
    this->counterInSyntacticSoup < (*this->currrentSyntacticSoup).size;
    this->counterInSyntacticSoup ++
  ) {
    counterReport ++;
    if (counterReport >= symbolsToIssueReport) {
      counterReport = 0;
      int64_t currentMilliseconds = global.getElapsedMilliseconds();
      if (currentMilliseconds - lastMilliseconds > minMillisecondsPerReport) {
        currentMilliseconds = lastMilliseconds;
        std::stringstream reportStream;
        reportStream << "Processed " << this->counterInSyntacticSoup << " out of " << (*this->currrentSyntacticSoup).size
        << " syntactic elements. ";
        theReport.report(reportStream.str());
      }
    }
    (*this->currentSyntacticStack).addOnTop((*this->currrentSyntacticSoup)[this->counterInSyntacticSoup]);
    int numTimesRulesCanBeAppliedWithoutStackDecrease = 0;
    int minStackSize = this->currentSyntacticStack->size ;
    while (this->applyOneRule()) {
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
  if ((*this->currentSyntacticStack).size == this->numEmptyTokensStart) {
    errorLog << "Non-meaningful/empty input detected (spacebar, enter characters only?).";
  } else if ((*this->currentSyntacticStack).size == this->numEmptyTokensStart + 1) {
    SyntacticElement& result = (*this->currentSyntacticStack)[this->numEmptyTokensStart];
    if (result.errorString == "" && result.controlIndex == this->conExpression()) {
      outputExpression = result.theData;
      success = true;
    } else if (result.errorString != "") {
      errorLog << "Syntax error with message: " << result.errorString;
    } else {
      errorLog << "Syntax error: your command simplifies to a single syntactic element but it is not an expression. <br>";
      errorLog << "It simplifies to:<br> " << this->toStringSyntacticStackHTMLTable(false);
    }
  } else {
    errorLog << "Syntax error: your command does not simplify to a single syntactic element. <br>";
    errorLog << "Instead it simplifies to:<br> " << this->toStringSyntacticStackHTMLTable(false);
  }
  if (outputErrors != nullptr) {
    *outputErrors = errorLog.str();
  }
  return success;
}

bool Calculator::applyOneRule() {
  MacroRegisterFunctionWithName("Calculator::applyOneRule");
  if (this->currentSyntacticStack->size <= this->numEmptyTokensStart) {
    return false;
  }
  const SyntacticElement& lastE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 1];
  const std::string& lastS = this->controlSequences[lastE.controlIndex];
  const SyntacticElement& secondToLastE = (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2];
  const std::string& secondToLastS = this->controlSequences[secondToLastE.controlIndex];
  if (secondToLastS == "Integer" && lastS != "Integer" && lastS != ".") {
    if (this->flagLogSyntaxRules) {
      this->parsingLog += "[Rule: digit to number]";
    }
    return this->replaceIntegerXbyEX();
  }
  if (
    lastS == " " &&
    secondToLastS == "\\" &&
    this->currentSyntacticStack->size >= this->numEmptyTokensStart + 2
  ) {
    this->popTopSyntacticStack();
    if (this->flagLogSyntaxRules) {
      this->parsingLog += "[Rule: remove \\ ]";
    }
    return this->popTopSyntacticStack();
  }
  if (lastS == " ") {
    if (this->flagLogSyntaxRules) {
      this->parsingLog += "[Rule: remove white space]";
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
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "<hr>" + this->toStringSyntacticStackHTMLTable(false);
  }
  if (secondToLastS == "%" && lastS == "LogParsing") {
    this->parsingLog += "<hr>" + this->toStringSyntacticStackHTMLTable(false);
    this->flagLogSyntaxRules = true;
    this->parsingLog += "[Rule: remove white space]";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "LogFull") {
    this->comments
    << "<hr>Requested a full log of the evaluation process.<hr>";
    this->flagLogFullTreeCrunching = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "UseBracketForIntervals") {
    this->flagUseBracketsForIntervals = true;
    this->comments << "Using brackets for intervals. ";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "DontUsePredefinedWordSplits") {
    this->flagUsePredefinedWordSplits = false;
    this->comments << "Using predefined word splits -for example xy is replaced by x y. ";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "WriteLatexDetails") {
    this->flagWriteLatexPlots = true;
    this->comments << "Creating LaTeX files.";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "PlotShowJavascriptOnly") {
    this->flagPlotShowJavascriptOnly = true;
    this->comments << "Plots show javascript only. ";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "PlotDetails") {
    this->flagPlotNoControls = false;
    this->comments << "Plot details on. ";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "HidePolynomialDataStructure") {
    this->flagHidePolynomialBuiltInTypeIndicator = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "LogEvaluation") {
    this->flagLogEvaluation = true;
    *this << "Log evaluation start. ";
    this->logTime();
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "UseLnInsteadOfLog") {
    this->comments << "Displaying complex logarithms as ln. ";
    this->flagUseLnInsteadOfLog = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "UseLnAbsInsteadOfLog") {
    this->flagUseLnAbsInsteadOfLogForIntegrationNotation = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "NumberColors") {
    if (!this->flagUseNumberColors) {
      *this << "<span style =\"color:blue\">Floating point numbers</span> are displayed in "
      << "<span style =\"color:blue\">blue</span>."
      << "<br><span style =\"color:red\">Algebraic numbers</span> "
      << "are displayed in <span style =\"color:red\">red</span>. "
      << "<br>Rational numbers are displayed in default color.";
    }
    this->flagUseNumberColors = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "LogCache") {
    this->flagLogCache = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "LogRules") {
    this->flagLogRules = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "ShowContext") {
    this->flagDisplayContext = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "NoLogarithmExponentShortcut") {
    this->atomsWhoseExponentsAreInterpretedAsFunctions.
    removeFirstOccurenceSwapWithLast(std::string("\\log"));
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "FullTree") {
    this->flagDisplayFullExpressionTree = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "HideLHS") {
    this->flagHideLHS = true;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "NoFrac") {
    this->flagUseFracInRationalLaTeX = false;
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (this->flagUsePredefinedWordSplits) {
    if (lastS == "Variable") {
      if (this->predefinedWordSplits.contains(this->operations.theKeys[lastE.theData.theData])) {
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
    return this->replaceXByO(this->opEmptySet());
  }
  if (lastS == "\\circ" && secondToLastS != "{" && secondToLastS != "(" && secondToLastS != "^")
    return this->replaceXByCon(this->conApplyFunction());
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
    if (lastE.theData.isOperationGiven("d")) {
      return this->replaceXXbyO(this->opDifferential());
    }
  }
  if (thirdToLastS == "\\text" && secondToLastS == "Expression") {
    if (secondToLastE.theData.isOperationGiven("d")) {
      return this->replaceXXYbyOY(this->opDifferential());
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
  //   return this->replaceEOEXByEX(secondToLastE.theData.format);
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
    return this->replaceXEEBy_OofEE(this->opBinom());
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
    secondToLastS == "Expression" && thirdToLastS == "-" &&
    fourthToLastS == "Expression" && this->allowsPlusInPreceding(lastS)
  ) {
    return this->replaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" && thirdToLastS == "-" &&
    fourthToLastS == "Expression" && lastS == "|" && fifthToLastS == "|"
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
    return this->replaceEXEXBy_OofEE_X(this->opBinom());
  }
  if (secondToLastS == "Expression" && thirdToLastS == "\\otimes" && fourthToLastS == "Expression" && this->allowsTensorInPreceding(lastS)) {
    return this->replaceEOEXByEX();
  }
  if (secondToLastS == "Expression" && thirdToLastS == "Expression" && fourthToLastS == "\\frac") {
    return this->replaceXEEXBy_OofEE_X(this->opDivide());
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
    this->atomsWhoseExponentsAreInterpretedAsFunctions.contains(fifthToLastS) && fourthToLastS == "^" &&
    thirdToLastS == "Expression" && secondToLastS == "Expression" &&
    this->allowsTimesInPreceding(lastS)
  ) {
    return this->replaceOOEEXbyEXpowerLike();
  }
  if (secondToLastS == "\\int" && lastS != "_") {
    return this->replaceOXbyEX();
  }
  if (thirdToLastS == "\\int" && secondToLastS == "_" && lastS == "Expression") {
    return this->replaceOXdotsXbyEXdotsX(2);
  }
  if (
    fourthToLastS == "\\int" && thirdToLastS == "_" &&
    secondToLastS == "Expression" && (lastS == "^" || lastS == "EndProgram")
  ) {
    return this->replaceOXdotsXbyEXdotsX(3);
  }
  if (
    fifthToLastS == "\\int" && fourthToLastS == "_" && thirdToLastS == "{}" &&
    secondToLastS == "^" && lastS == "{}"
  ) {
    this->popTopSyntacticStack();
    this->popTopSyntacticStack();
    this->popTopSyntacticStack();
    this->replaceOXbyEX();
    return this->popTopSyntacticStack();
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
    if (this->outerTimesToFunctionApplication(
      *this,
      (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2].theData,
      impliedFunApplication
    )) {
      (*this->currentSyntacticStack)[(*this->currentSyntacticStack).size - 2].theData = impliedFunApplication;
      if (this->flagLogSyntaxRules) {
        this->parsingLog += "[Rule: implied function application]";
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
  if (thirdToLastS == "(" && secondToLastS == "\\circ" && lastS == ")") {
    return this->replaceOXbyEX();
  }
  if (thirdToLastS == "{" && secondToLastS == "\\circ" && lastS == "}") {
    return this->replaceOXbyEX();
  }
  if (secondToLastS == "^" && lastS == "\\circ") {
    return this->replaceObyE();
  }
  if (this->flagUseBracketsForIntervals) {
    if (thirdToLastS == "(" && secondToLastS == "Expression" && lastS == "]") {
      return this->replaceXEXByEContainingOE(this->opIntervalRightClosed());
    }
    if (thirdToLastS == "[" && secondToLastS == "Expression" && lastS == ")") {
      return this->replaceXEXByEContainingOE(this->opIntervalLeftClosed());
    }
    if (thirdToLastS == "[" && secondToLastS == "Expression" && lastS == "]") {
      return this->replaceXEXByEContainingOE(this->opIntervalClosed());
    }
    if (this->flagUseBracketsForIntervals) {
      if (secondToLastE.theData.isSequenceNElements(2)) {
        if (
          this->isSeparatorFromTheLeftForInterval(fourthToLastS) &&
          thirdToLastS == "(" &&
          secondToLastS == "Sequence" &&
          lastS == ")"
        ) {
          return this->replaceXEXByEContainingOE(this->opIntervalOpen());
        }
      }
    }
  }
  if (thirdToLastS == "\"" && secondToLastS == "Expression" && lastS == "\"") {
    return this->replaceXEXByEContainingOE(this->opQuote());
  }
  if (secondToLastS == "\"" && lastS == "\"") {
    return this->replaceXXByEmptyString();
  }
  if (lastS == "Expression" && secondToLastS == "~" && thirdToLastS == "Expression") {
    return this->replaceEOEByE();
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
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && this->isSeparatorFromTheRightForDefinition(lastS) && secondToLastS == "Expression" && thirdToLastS == "\\leq" && fourthToLastS == "Expression") {
    return this->replaceEOEXByEX();
  }
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && this->isSeparatorFromTheRightForDefinition(lastS) && secondToLastS == "Expression" && thirdToLastS == ">" && fourthToLastS == "Expression") {
    return this->replaceEOEXByEX();
  }
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && this->isSeparatorFromTheRightForDefinition(lastS) && secondToLastS == "Expression" && thirdToLastS == "<" && fourthToLastS == "Expression") {
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
    lastE.theData.size() == 0 &&
    lastE.theData.theData == this->opLisT()
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
    this->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(thirdToLastS) &&
    secondToLastS == "Expression" && this->allowsTimesInPreceding(lastS)
  ) {
    return this->replaceOEXByEX();
  }
  if (
    this->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(fourthToLastS) &&
    thirdToLastS == "^" && secondToLastS == "Expression" && this->isSeparatorFromTheRightGeneral(lastS)
  ) {
    return this->replaceC1C2Eby_C2ofC1E();
  }
  if (
    this->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(fourthToLastS) &&
    thirdToLastS == "{}" && secondToLastS == "Expression" && this->allowsApplyFunctionInPreceding(lastS)
  ) {
    return this->replaceOXEXByEX();
  }
  if (
    this->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(secondToLastS) &&
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
    return this->replaceXByO(this->opInfinity());
  }
  if (lastS == "pi") {
    return this->replaceXByO(this->opPi());
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
      (this->flagUseBracketsForIntervals && lastS == "]")
    )
  ) {
    return this->replaceSequenceUXEYBySequenceZY(this->conSequence());
  }
  if (
    (thirdToLastS != "[" || this->flagUseBracketsForIntervals)&&
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
        this->flagUseBracketsForIntervals && (
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
  if (!this->flagUseBracketsForIntervals) {
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

