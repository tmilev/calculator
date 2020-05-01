// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"

SyntacticElement Calculator::GetEmptySyntacticElement() {
  SyntacticElement result;
  result.controlIndex = this->controlSequences.getIndex(" ");
  result.theData.reset(*this);
  return result;
}

std::string SyntacticElement::ToStringHumanReadable(Calculator& theBoss, bool includeLispifiedExpressions) const {
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
    out <<  "<tr><td style =\"color:#AAAAAA\">" << this->theData.toStringFull() << "</td></tr>";
  }
  if (this->errorString != "") {
    out << "<tr><td>" << this->errorString << "</td></tr>";
  }
  out << "</table>";
  return out.str();
}

Calculator::EvaluationStats::EvaluationStats() {
  this->expressionEvaluated = 0;
  this->maximumCallsBeforeReportGeneration = 5000;
  this->callsSinceReport = 0;
}

void Calculator::resetFrequentConstants() {

}

void Calculator::reset() {
  this->MaxAlgTransformationsPerExpression = 100;
  this->MaxRuleStacksCached = 500;
  this->MaxCachedExpressionPerRuleStack = 100000;
  this->MaxRecursionDeptH = 10000;
  this->RecursionDeptH = 0;
  this->NumErrors = 0;
  this->NumListsStart               = - 1;
  this->NumListResizesStart         = - 1;
  this->NumHashResizesStart         = - 1;
  this->NumSmallAdditionsStart      = - 1;
  this->NumSmallMultiplicationsStart = - 1;
  this->NumSmallGCDcallsStart       = - 1;
  this->NumLargeAdditionsStart      = - 1;
  this->NumLargeMultiplicationsStart = - 1;
  this->NumLargeGCDcallsStart       = - 1;
  this->LastLogEvaluationTime = global.getElapsedSeconds();
  this->DepthRecursionReached = 0;
  this->flagWriteLatexPlots = false;
  this->flagLogSyntaxRules = false;
  this->flagLogEvaluatioN = false;
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
  this->flagNoApproximationS = false;
  this->flagDefaultRulesWereTamperedWith = false;
  this->flagUsePredefinedWordSplits = true;
  this->flagPlotNoControls = true;
  this->flagPlotShowJavascriptOnly = false;
  this->flagHasGraphics = false;
  this->flagUseBracketsForIntervals = false;

  this->MaxLatexChars = 2000;
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

  this->syntacticSouP.setSize(0);
  this->syntacticStacK.setSize(0);
  this->flagTimeLimitErrorDetected = false;
  this->flagFirstErrorEncountered = false;
  this->flagMaxTransformationsErrorEncountered = false;
  this->flagMaxRecursionErrorEncountered = false;
  this->flagAbortComputationASAP = false;
  this->flagDisplayContext = false;
  this->EvaluatedExpressionsStack.clear();
  this->theCruncherIds.clear();
  this->theCruncherS.setSize(0);
  this->syntaxErrors = "";
  this->evaluationErrors.setSize(0);
  this->CurrentSyntacticStacK = &this->syntacticStacK;
  this->CurrrentSyntacticSouP = &this->syntacticSouP;
  this->cachedExpressions.clear();
  this->imagesCachedExpressions.setSize(0);
  this->theProgramExpression.reset(*this);
  this->RuleStackCacheIndex = - 1;
  this->RuleStack.reset(*this,this->MaxRuleStacksCached);
  this->cachedRuleStacks.clear();
  // The expression container must be cleared last!
  this->theExpressionContainer.clear();
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
  this->AddOperationNoRepetitionAllowed("");

  this->AddOperationNoRepetitionAllowed("=");
  this->AddOperationNoRepetitionAllowed(";");
  this->AddOperationNoRepetitionAllowed("if=");
  this->AddOperationNoRepetitionAllowed("+");
  this->AddOperationNoRepetitionAllowed("-");
  this->AddOperationNoRepetitionAllowed("/");
  this->AddOperationNoRepetitionAllowed("*");
  this->AddOperationNoRepetitionAllowed("!");
  this->AddOperationNoRepetitionAllowed("mod");
  this->AddOperationNoRepetitionAllowed("and");
  this->AddOperationNoRepetitionAllowed("or");
  this->AddOperationNoRepetitionAllowed("\\times");
  this->AddOperationNoRepetitionAllowed("\\otimes");
  this->AddOperationNoRepetitionAllowed("\\oplus");
  this->AddOperationNoRepetitionAllowed("\\binom");
  this->AddOperationNoRepetitionAllowed("\\sqrt");
  this->AddOperationNoRepetitionAllowed("\\in");
  this->AddOperationNoRepetitionAllowed("[]");
  this->AddOperationNoRepetitionAllowed("=:");
  this->AddOperationNoRepetitionAllowed("^");
  this->AddOperationNoRepetitionAllowed("\\circ");
  this->AddOperationNoRepetitionAllowed("\\geq");
  this->AddOperationNoRepetitionAllowed("\\leq");
  this->AddOperationNoRepetitionAllowed(">");
  this->AddOperationNoRepetitionAllowed("<");
  this->AddOperationNoRepetitionAllowed("==");
  this->AddOperationNoRepetitionAllowed("===");
  this->AddOperationNoRepetitionAllowed("\\cup");
  this->AddOperationNoRepetitionAllowed("\\sqcup");
  this->AddOperationNoRepetitionAllowed("\\cap");
  this->AddOperationNoRepetitionAllowed("Error");
  this->AddOperationNoRepetitionAllowed("Sequence");
  this->AddOperationNoRepetitionAllowed("Matrix");
  this->AddOperationNoRepetitionAllowed("Context");
  this->AddOperationNoRepetitionAllowed("|");
  this->AddOperationNoRepetitionAllowed("\"");
  this->AddOperationNoRepetitionAllowed("PolyVars");
  this->AddOperationNoRepetitionAllowed("DiffOpVars");
  this->AddOperationNoRepetitionAllowed("\\to");
  this->AddOperationNoRepetitionAllowed("if");
  this->AddOperationNoRepetitionAllowed("\\lim");
  this->AddOperationNoRepetitionAllowed("LogBase");
  this->AddOperationNoRepetitionAllowed("\\int");

  this->AddOperationBuiltInType("Rational");
  this->AddOperationBuiltInType("EltZmodP");
  this->AddOperationBuiltInType("Double");
  this->AddOperationBuiltInType("AlgebraicNumber");
  this->AddOperationBuiltInType("PolynomialRational");
  this->AddOperationBuiltInType("PolynomialOverANs");
  this->AddOperationBuiltInType("PolynomialModuloInteger");
  this->AddOperationBuiltInType("PolynomialModuloPolynomialModuloInteger");
  this->AddOperationBuiltInType("RationalFunction");
  this->AddOperationBuiltInType("string");
  this->AddOperationBuiltInType("ElementUEoverRF");
  this->AddOperationBuiltInType("ElementTensorGVM");
  this->AddOperationBuiltInType("CharSSAlgMod");
  this->AddOperationBuiltInType("SemisimpleLieAlg");
  this->AddOperationBuiltInType("LittelmannPath");
  this->AddOperationBuiltInType("LRO");
  this->AddOperationBuiltInType("MatrixRational");
  this->AddOperationBuiltInType("MatrixDouble");
  this->AddOperationBuiltInType("MatrixAlgebraic");
  this->AddOperationBuiltInType("MatrixTensorRational");
  this->AddOperationBuiltInType("MatrixRF");
  this->AddOperationBuiltInType("MatrixPolynomialRational");
  this->AddOperationBuiltInType("CalculusPlot");
  this->AddOperationBuiltInType("SemisimpleSubalgebras");
  this->AddOperationBuiltInType("CandidateSSsubalgebra");
  this->AddOperationBuiltInType("WeylGroup");
  this->AddOperationBuiltInType("ElementWeylGroup");
  this->AddOperationBuiltInType("HyperoctahedralGroupRepresentation");
  this->AddOperationBuiltInType("ElementHyperoctahedral");
  this->AddOperationBuiltInType("WeylGroupRep");
  this->AddOperationBuiltInType("WeylGroupVirtualRep");
  this->AddOperationBuiltInType("ElementWeylAlgebra");
  this->AddOperationBuiltInType("weightLieAlg");
  this->AddOperationBuiltInType("weightLieAlgPoly");
  this->AddOperationBuiltInType("EllipticCurveElementsRational");
  this->AddOperationBuiltInType("EllipticCurveElementsZmodP");
  this->AddOperationBuiltInType("userInputTextBox");

  this->initializeOperationsInterpretedAsFunctionsMultiplicatively();

  // Empty token comes first.
  this->controlSequences.addOnTopNoRepetitionMustBeNew(" ");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("{{}}");
  this->controlSequences.addOnTopNoRepetitionMustBeNew("Variable");
  this->controlSequences.addOnTop(this->operations.theKeys);//all operations defined up to this point are also control sequences
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
  this->TotalNumpatternMatchedPerformed = 0;
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

  this->RuleStack.reset(*this, 100);
  this->RuleStack.addChildAtomOnTop(this->opEndStatement());
  this->cachedRuleStacks.clear();
  this->RuleStackCacheIndex = 0;
  this->cachedRuleStacks.addOnTop(this->RuleStack);
  this->NumPredefinedAtoms = this->operations.size(); //<-operations added up to this point are called ``operations''
  this->CheckConsistencyAfterInitialization();
}

bool Calculator::CheckPredefinedFunctionNameRepetitions() {
  MacroRegisterFunctionWithName("Calculator::CheckPredefinedFunctionNameRepetitions");
  HashedList<std::string, MathRoutines::HashString> ruleIds;
  for (int i = 0; i < this->operations.size(); i ++) {
    MemorySaving<Calculator::OperationHandlers>& current = this->operations.theValues[i];
    if (current.IsZeroPointer()) {
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

bool Calculator::ReplaceOXEXEXEXByE() {
  SyntacticElement& opElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 8];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 6];
  SyntacticElement& middleE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.theData);
  newExpr.addChildOnTop(middleE.theData);
  newExpr.addChildOnTop(rightE.theData);
  opElt.theData = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(7);
}

bool Calculator::ReplaceSqrtEXByEX() {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& argument = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->opSqrt());
  newExpr.addChildOnTop(this->ETwo());
  newExpr.addChildOnTop(argument.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  return this->DecreaseStackExceptLast(1);
}

bool Calculator::ReplaceSqrtEXXByEXX() {
  int lastSize = (*this->CurrentSyntacticStacK).size;
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[lastSize - 4];
  SyntacticElement& argument = (*this->CurrentSyntacticStacK)[lastSize - 3];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->opSqrt());
  newExpr.addChildOnTop(this->ETwo());
  newExpr.addChildOnTop(argument.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  (*this->CurrentSyntacticStacK)[lastSize - 3] = (*this->CurrentSyntacticStacK)[lastSize - 2];
  (*this->CurrentSyntacticStacK)[lastSize - 2] = (*this->CurrentSyntacticStacK)[lastSize - 1];
  (*this->CurrentSyntacticStacK).setSize(lastSize - 1);
  return true;
}

bool Calculator::ReplaceSqrtXEXByEX() {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& argument = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->opSqrt());
  newExpr.addChildOnTop(this->ETwo());
  newExpr.addChildOnTop(argument.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  return this->DecreaseStackExceptLast(2);
}

bool Calculator::ReplaceOXEXEByE() {
  SyntacticElement& opElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 5];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.theData);
  newExpr.addChildOnTop(rightE.theData);
  opElt.theData = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(4);
}

bool Calculator::ReplaceOXEXEXByEX() {
  SyntacticElement& opElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 6];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.theData);
  newExpr.addChildOnTop(rightE.theData);
  opElt.theData = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->DecreaseStackExceptLast(4);
}

bool Calculator::ReplaceOXEXEXXByEXX() {
  SyntacticElement& opElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 7];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 5];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.theData);
  newExpr.addChildOnTop(rightE.theData);
  opElt.theData = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->DecreaseStackExceptLastTwo(4);
}

void Calculator::LogPublicError(const std::string& theError) {
  if (this->errorsPublic.str() != "") {
    this->errorsPublic << theError;
  }
}

bool Calculator::DecreaseStackExceptLast(int decrease) {
  if (decrease <= 0) {
    return true;
  }
  if ((*this->CurrentSyntacticStacK).size - decrease <= 0) {
    global.fatal << "Bad stack decrease amount. " << global.fatal;
  }
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - decrease - 1] =
  *this->CurrentSyntacticStacK->lastObject();
  (*this->CurrentSyntacticStacK).setSize((*this->CurrentSyntacticStacK).size - decrease);
  return true;
}

bool Calculator::DecreaseStackExceptLastTwo(int decrease) {
  if (decrease <= 0) {
    return true;
  }
  if ((*this->CurrentSyntacticStacK).size - decrease <= 0) {
    global.fatal << "Bad stack decrease amount. " << global.fatal;
  }
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-decrease - 2] =
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-decrease - 1] =
  *this->CurrentSyntacticStacK->lastObject();
  (*this->CurrentSyntacticStacK).setSize((*this->CurrentSyntacticStacK).size - decrease);
  return true;
}

bool Calculator::ReplaceOXEEXByEX() {
  SyntacticElement& opElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 5];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  Expression newExpr(*this);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.theData);
  newExpr.addChildOnTop(rightE.theData);
  opElt.theData = newExpr;
  opElt.controlIndex = this->conExpression();
  return this->DecreaseStackExceptLast(3);
}

bool Calculator::ReplaceOXXEXEXEXByE() {
  SyntacticElement& opElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 9];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 6];
  SyntacticElement& middleE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.addChildOnTop(leftE.theData);
  newExpr.addChildOnTop(middleE.theData);
  newExpr.addChildOnTop(rightE.theData);
  opElt.theData = newExpr;
  opElt.controlIndex = this->conExpression();
  this->DecreaseStackSetCharacterRangeS(8);
  return true;
}

bool Calculator::ReplaceOXEXByEX() {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(left.controlIndex));
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  return this->DecreaseStackExceptLast(2);
}

bool Calculator::ReplaceOXXByEXX() {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceOXXByEXX]";
  }
  left.theData.makeAtom(this->GetOperationIndexFromControlIndex(left.controlIndex), *this);
  left.controlIndex = this->conExpression();
  return true;
}

bool Calculator::ReplaceOXEByE() {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(left.controlIndex));
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(2);
}

bool Calculator::ReplaceOEByE() {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(left.controlIndex));
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(1);
}

bool Calculator::ReplaceOEXByEX() {
  SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceOEXByEX]";
  }
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.addChildOnTop(right.theData);
  middle.theData = newExpr;
  middle.controlIndex = this->conExpression();
  this->DecreaseStackExceptLast(1);
  return true;
}

bool Calculator::ReplaceXXXByCon(int theCon) {
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3].controlIndex = theCon;
  this->DecreaseStackSetCharacterRangeS(2);
  return true;
}

bool Calculator::ReplaceXXXByConCon(int con1, int con2) {
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2].controlIndex = con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3].controlIndex = con2;
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool Calculator::ReplaceXXXXXByCon(int theCon) {
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 5].controlIndex = theCon;
  this->DecreaseStackSetCharacterRangeS(4);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceXXXXXByCon]";
  }
  return true;
}

bool Calculator::ReplaceXXXXXByConCon(int con1, int con2) {
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4].controlIndex = con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 5].controlIndex = con1;
  this->DecreaseStackSetCharacterRangeS(3);
  return true;
}

bool Calculator::ReplaceXXXXByConCon(int con1, int con2) {
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3].controlIndex = con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4].controlIndex = con1;
  this->DecreaseStackSetCharacterRangeS(2);
  return true;
}

bool Calculator::ReplaceXXXXByCon(int con1) {
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4].controlIndex = con1;
  this->DecreaseStackSetCharacterRangeS(3);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceXXXXByCon]";
  }
  return true;
}

bool Calculator::ReplaceXByCon(int theCon) {
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1].controlIndex = theCon;
  return true;
}

bool Calculator::ReplaceXByO(int theOperation) {
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1].controlIndex = this->conExpression();
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1].theData.makeAtom(theOperation, *this);
  return true;
}

bool Calculator::ReplaceXByConCon(int con1, int con2) {
  (*this->CurrentSyntacticStacK).setSize((*this->CurrentSyntacticStacK).size + 1);
  (*this->CurrentSyntacticStacK).lastObject()->theData.reset(*this);
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2].controlIndex = con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1].controlIndex = con2;
  return true;
}

bool Calculator::ReplaceOEXByE() {
  SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceOEXByE]";
  }
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.addChildOnTop(right.theData);
  middle.theData = newExpr;
  middle.controlIndex = this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(2);
}

bool Calculator::ReplaceXXByEmptyString() {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  Expression newExpr;
  left.theData.assignValue(std::string(""), *this);
  left.controlIndex = this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(1);
}

bool Calculator::ReplaceEOByE() {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(right.controlIndex));
  newExpr.addChildOnTop(left.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  this->DecreaseStackSetCharacterRangeS(1);
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

void Calculator::ParseFillDictionary(const std::string& input) {
  MacroRegisterFunctionWithName("Calculator::ParseFillDictionary");
  this->ParseFillDictionary(input, *this->CurrrentSyntacticSouP);
  SyntacticElement currentElement;
  currentElement.theData.reset(*this);
  currentElement.controlIndex = this->conEndProgram();
  (*this->CurrrentSyntacticSouP).addOnTop(currentElement);
}

bool Calculator::ShouldSplitOutsideQuotes(const std::string& left, char right) {
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

void Calculator::ParseFillDictionary(const std::string& input, List<SyntacticElement>& output) {
  MacroRegisterFunctionWithName("Calculator::ParseFillDictionary");
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
      shouldSplit = this->ShouldSplitOutsideQuotes(current, LookAheadChar);
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
      currentElement.theData.makeAtom(this->AddOperationNoRepetitionOrReturnIndexFirst(current), *this);
      output.addOnTop(currentElement);
    }
    current = "";
  }
}

int Calculator::GetOperationIndexFromControlIndex(int controlIndex) {
  return this->operations.getIndex(this->controlSequences[controlIndex]);
}

int Calculator::GetExpressionIndex() {
  return this->controlSequences.getIndex("Expression");
}

bool Calculator::ReplaceXXbyE() {
  this->ReplaceXXbyEX();
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool Calculator::ReplaceXXXbyE() {
  this->ReplaceXXbyEX();
  return this->ReplaceXEXByE();
}

bool Calculator::ReplaCeOXdotsXbyEXdotsX(int numXs) {
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1 - numXs];
  theElt.theData.makeAtom(this->GetOperationIndexFromControlIndex(theElt.controlIndex), *this);
  if (this->flagLogSyntaxRules) {
    std::stringstream out;
    out << "[Rule: Calculator::ReplaCeOXdotsXbyEXdotsX: " << numXs << "]";
    this->parsingLog += out.str();
  }
  theElt.controlIndex = this->conExpression();
  return true;
}

bool Calculator::ReplaceOXbyEX() {
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  theElt.theData.makeAtom(this->GetOperationIndexFromControlIndex(theElt.controlIndex), *this);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceOXbyEX]";
  }
  return this->ReplaceXXbyEX();
}

bool Calculator::ReplaCeObyE() {
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  theElt.theData.makeAtom(this->GetOperationIndexFromControlIndex(theElt.controlIndex), *this);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceObyE]";
  }
  theElt.controlIndex = this->conExpression();
  return true;
}

bool Calculator::ReplaceXXbyEX() {
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  theElt.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceXXbyEX]";
  }
  return true;
}

bool Calculator::ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence() {
  SyntacticElement& theSequenceElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  SyntacticElement& theFunctionElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 5];
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
  return this->DecreaseStackSetCharacterRangeS(4);
}

bool Calculator::ReplaceXEXByE() {
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3] =
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3].controlIndex = this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(2);
}

bool Calculator::ReplaceXEXByEcontainingOE(int inputOpIndex) {
  SyntacticElement& outputElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& inputElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceXEXByEcontainingOE]";
  }
  outputElt.theData.reset(*this, 2);
  outputElt.theData.addChildAtomOnTop(inputOpIndex);
  outputElt.theData.addChildOnTop(inputElt.theData);
  outputElt.controlIndex = this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(2);
}

bool Calculator::ReplaceXXByEEmptySequence() {
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  theElt.theData.reset(*this, 1);
  theElt.theData.addChildAtomOnTop(this->opSequence());
  theElt.controlIndex = this->conExpression();
  return this->DecreaseStackSetCharacterRangeS(1);
}

bool Calculator::isBoundVariableInContext(int inputOp) {
  return this->BoundVariablesInContext.contains(inputOp);
}

bool Calculator::IsNonBoundVarInContext(int inputOp) {
  return this->NonBoundVariablesInContext.contains(inputOp);
}

bool Calculator::ReplaceXXVXdotsXbyE_BOUND_XdotsX(int numXs) {
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - numXs - 1];
  int theBoundVar = theElt.theData.theData;
  if (this->IsNonBoundVarInContext(theBoundVar)) {
    std::stringstream out;
    out << "Syntax error. In the same syntactic scope, the string "
    << this->operations.theKeys[theBoundVar]
    << " is first used to denote a non-bound variable "
    << "but later to denote a bound variable. This is not allowed. ";
    theElt.errorString = out.str();
    theElt.controlIndex = this->conError();
    this->DecreaseStackSetCharacterRangeS(numXs);
    this->ReplaceXXYByY();
    return true;
  }
  if (!this->isBoundVariableInContext(theBoundVar)) {
    this->BoundVariablesInContext.addOnTopNoRepetition(theBoundVar);
  }
  theElt.theData.reset(*this, 2);
  theElt.theData.addChildAtomOnTop(this->opBind());
  theElt.theData.addChildAtomOnTop(theBoundVar);
  theElt.controlIndex = this->conExpression();
  this->DecreaseStackSetCharacterRangeS(numXs);
  this->ReplaceXXYByY();
  return true;
}

bool Calculator::ReplaceVXdotsXbyE_NONBOUND_XdotsX(int numXs) {
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1 - numXs];
  int theBoundVar = theElt.theData.theData;
  if (this->isBoundVariableInContext(theBoundVar)) {
    theElt.theData.reset(*this, 2);
    theElt.theData.addChildAtomOnTop(this->opBind());
    theElt.theData.addChildAtomOnTop(theBoundVar);
  } else {
    theElt.theData.makeAtom(theBoundVar, *this);
    if (!this->IsNonBoundVarInContext(theBoundVar)) {
      this->NonBoundVariablesInContext.addOnTop(theBoundVar);
    }
  }
  theElt.controlIndex = this->conExpression();
  return true;
}

bool Calculator::ReplaceOOEEXbyEXpowerLike() {
  SyntacticElement& outerO = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& innerO = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 5];
  SyntacticElement& innerArg = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  SyntacticElement& outerArg = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceOOEEXbyEXpowerLike]";
  }
  Expression newInnerE(*this), newFinalE(*this);
  newInnerE.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(innerO.controlIndex));
  newInnerE.addChildOnTop(innerArg.theData);
  newFinalE.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(outerO.controlIndex));
  newFinalE.addChildOnTop(newInnerE);
  newFinalE.addChildOnTop(outerArg.theData);

  innerO.theData = newFinalE;
  innerO.controlIndex = this->conExpression();
  return this->DecreaseStackExceptLast(3);
}

bool Calculator::ReplaceCXByE() {
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  theElt.controlIndex = this->conExpression();
  return this->PopTopSyntacticStack();
}

bool Calculator::ReplaceCXByEX() {
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  theElt.controlIndex = this->conExpression();
  return true;
}

bool Calculator::ReplaceXdotsXByMatrixStart(int numXes) {
  SyntacticElement& currentElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - numXes];
  currentElt.dataList.setExpectedSize(10);
  currentElt.dataList.setSize(1);
  currentElt.dataList.lastObject()->makeSequence(*this);
  currentElt.controlIndex = this->conMatrixStart();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceXdotsXByMatrixStart]";
  }
  return this->DecreaseStackSetCharacterRangeS(numXes - 1);
}

bool Calculator::ReplaceMatrixXByE() {
  SyntacticElement& theMatElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  Matrix<Expression> theMat;
  int numRows = theMatElt.dataList.size;
  int numCols = 0;
  for (int i = 0; i < theMatElt.dataList.size; i ++) {
    numCols = MathRoutines::Maximum(theMatElt.dataList[i].size() - 1, numCols);
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
    this->parsingLog += "[Rule: Calculator::ReplaceMatrixXByE]";
  }
  return this->DecreaseStackSetCharacterRangeS(1);
}

bool Calculator::ReplaceMatrixEXByMatrixNewRow() {
  SyntacticElement& theMatElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  theMatElt.dataList.lastObject()->addChildOnTop(theElt.theData);
  theMatElt.dataList.setSize(theMatElt.dataList.size + 1);
  theMatElt.dataList.lastObject()->makeSequence(*this);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceMatrixEXByMatrixNewRow]";
  }
  return this->DecreaseStackSetCharacterRangeS(2);
}

bool Calculator::ReplaceMatrixEXByMatrix() {
  SyntacticElement& theMatElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  theMatElt.dataList.lastObject()->addChildOnTop(theElt.theData);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceMatrixEXByMatrix]";
  }
  return this->DecreaseStackSetCharacterRangeS(2);
}

bool Calculator::ReplaceMatrixEXByMatrixX() {
  SyntacticElement& theMatElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  theMatElt.dataList.lastObject()->addChildOnTop(theElt.theData);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceMatrixEXByMatrixX]";
  }
  return this->DecreaseStackExceptLast(1);
}

bool Calculator::ReplaceAXbyEX() {
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  theElt.controlIndex = this->conExpression();
  return true;
}

std::string SyntacticElement::GetIntegerStringCrashIfNot(Calculator& owner) {
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

bool Calculator::ReplaceIntegerDotIntegerByE() {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  std::string afterDecimalPoint = right.GetIntegerStringCrashIfNot(*this);
  std::string beforeDecimalPoint = left.GetIntegerStringCrashIfNot(*this);
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
  this->DecreaseStackSetCharacterRangeS(2);
  return true;
}

bool Calculator::ReplaceIntegerXbyEX() {
  SyntacticElement& theElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  theElt.controlIndex = this->conExpression();
  Rational value;
  value.assignString(theElt.theData.getValue<std::string>());
  theElt.theData.assignValue(value, *this);
  return true;
}

std::string Calculator::ToStringIsCorrectAsciiCalculatorString(const std::string& input) {
  std::stringstream out;
  HashedList<char, MathRoutines::HashChar> theBadChars;
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

bool Calculator::AllowsApplyFunctionInPreceding(const std::string& lookAhead) {
  return
  lookAhead != "{" && lookAhead != "_" &&
  lookAhead != "\\circ" && lookAhead != "{}" &&
  lookAhead != "$";
}

bool Calculator::ReplaceSequenceUXEYBySequenceZY(int theControlIndex) {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& afterleft = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  left.theData.addChildOnTop(right.theData);
  left.controlIndex = theControlIndex;
  afterleft = *this->CurrentSyntacticStacK->lastObject();
  this->DecreaseStackExceptLast(2);
  return true;
}

bool Calculator::ReplaceYXBySequenceX(int theControlIndex) {
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceYXBySequenceX]";
  }
  return this->ReplaceYXdotsXBySequenceYXdotsX(theControlIndex, 1);
}

bool Calculator::ReplaceYBySequenceY(int theControlIndex) {
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceYBySequenceY]";
  }
  return this->ReplaceYXdotsXBySequenceYXdotsX(theControlIndex, 0);
}

bool Calculator::ReplaceXXYBySequenceY(int theControlIndex) {
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceXXYBySequenceY]";
  }
  this->ReplaceYBySequenceY(theControlIndex);
  return this->ReplaceXXYByY();
}

bool Calculator::ReplaceXXYXBySequenceYX(int theControlIndex) {
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceXXYXBySequenceYX]";
  }
  this->ReplaceYXdotsXBySequenceYXdotsX(theControlIndex, 1);
  return this->ReplaceXXYXByYX();
}

bool Calculator::ReplaceSequenceXEBySequence(int theControlIndex) {
  SyntacticElement& left =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  left.theData.addChildOnTop(right.theData);
  left.controlIndex = theControlIndex;
  this->DecreaseStackSetCharacterRangeS(2);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceSequenceXEBySequence]";
  }
  return true;
}

bool Calculator::ReplaceYXdotsXBySequenceYXdotsX(int theControlIndex, int numXs) {
  SyntacticElement& main = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - numXs - 1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(this->opSequence());
  newExpr.addChildOnTop(main.theData);
  main.theData = newExpr;
  main.controlIndex = theControlIndex;
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceYXdotsXBySequenceYXdotsX]";
  }
  return true;
}

bool Calculator::ReplaceEXEBySequence(int theControlIndex) {
  SyntacticElement& left =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->opSequence());
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = theControlIndex;
  this->DecreaseStackSetCharacterRangeS(2);
  return true;
}

bool Calculator::ReplaceEEBy_CofEE(int theControlIndex) {
  SyntacticElement& left =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool Calculator::ReplaceEOXbyEX() {
  SyntacticElement& left =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& opElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  int theOp = this->GetOperationIndexFromControlIndex(opElt.controlIndex);
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildAtomOnTop(theOp);
  newExpr.addChildOnTop(left.theData);
  left.theData = newExpr;
  this->DecreaseStackExceptLast(1);
  return true;
}

bool Calculator::ReplaceVbyVdotsVAccordingToPredefinedWordSplits() {
  MacroRegisterFunctionWithName("Calculator::ReplaceVbyVdotsVAccordingToPredefinedWordSplits");
  SyntacticElement& theE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  const std::string& currentVar = this->operations.theKeys[theE.theData.theData];
  if (!this->predefinedWordSplits.contains(currentVar)) {
    global.fatal << "Predefined word splits array does not contain the variable: " << theE.theData.toString()
    << ". This should not happen in the body of this function. " << global.fatal;
  }
  List<std::string>& theSplit = this->predefinedWordSplits.GetValueCreate(currentVar);
  SyntacticElement newElt;
  this->PopTopSyntacticStack();
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
    newElt.theData.makeAtom(this->AddOperationNoRepetitionOrReturnIndexFirst(theSplit[i]), *this);
    newElt.controlIndex = this->controlSequences.getIndex(theSplit[i]);
    if (newElt.controlIndex == - 1) {
      newElt.controlIndex = this->conVariable();
    }
    (*this->CurrentSyntacticStacK).addOnTop(newElt);
  }
  return true;
}

bool Calculator::ReplaceEEXBy_CofEE_X(int theControlIndex) {
  SyntacticElement& left =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceEEXBy_CofEE_X]";
  }
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  this->DecreaseStackExceptLast(1);
  return true;
}

bool Calculator::ReplaceEXXEXEBy_CofEEE(int theControlIndex) {
  SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 6];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(middle.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  this->DecreaseStackSetCharacterRangeS(5);
  return true;
}

bool Calculator::ReplaceEXXEXEXBy_CofEEE_X(int theControlIndex) {
  SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 7];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(middle.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  this->DecreaseStackExceptLast(5);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceEXXEXEXBy_CofEEE_X]";
  }
  return true;
}

bool Calculator::ReplaceUnderscoreEPowerEbyLimits() {
  SyntacticElement& bottom =
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& top =
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->opLimitBoundary());
  newExpr.addChildOnTop(bottom.theData);
  newExpr.addChildOnTop(top.theData);
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4].theData = newExpr;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4].controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceUnderscoreEPowerEbyLimits]";
  }
  return this->DecreaseStackSetCharacterRangeS(3);
}

bool Calculator::ReplacePowerEUnderScoreEbyLimits() {
  SyntacticElement& bottom =
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& top =
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  MathRoutines::swap(bottom, top);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplacePowerEUnderScoreEbyLimits]";
  }
  return this->ReplaceUnderscoreEPowerEbyLimits();
}

bool Calculator::ReplaceXEXEXBy_CofEE(int theControlIndex) {
  SyntacticElement& lefT = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  SyntacticElement& result = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 5];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.addChildOnTop(lefT.theData);
  newExpr.addChildOnTop(right.theData);
  result.theData = newExpr;
  result.controlIndex = this->conExpression();
  this->DecreaseStackSetCharacterRangeS(4);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceXEXEXBy_CofEE]";
  }
  return true;
}

bool Calculator::ReplaceEXdotsXbySsXdotsX(int numDots) {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - numDots - 1];
  bool found = false;
  for (int i = (*this->CurrentSyntacticStacK).size - numDots - 2; i >= 0; i --) {
    SyntacticElement& current = (*this->CurrentSyntacticStacK)[i];
    if (current.numBoundVariablesInherited >= 0 && current.numNonBoundVariablesInherited >= 0) {
      this->NonBoundVariablesInContext.setSize(current.numNonBoundVariablesInherited);
      this->BoundVariablesInContext.setSize(current.numBoundVariablesInherited);
      found = true;
      break;
    }
  }
  if (!found) {
    this->NonBoundVariablesInContext.clear();
    this->BoundVariablesInContext.clear();
  }
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.reserve(2);
  newExpr.addChildAtomOnTop(this->opEndStatement());
  newExpr.addChildOnTop(left.theData);
  left.theData = newExpr;
  left.controlIndex = this->conSequenceStatements();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceEXdotsXbySsXdotsX]";
  }
  return true;
}

bool Calculator::ReplaceSsSsXdotsXbySsXdotsX(int numDots) {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - numDots - 2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - numDots - 1];
  if (!left.theData.startsWith(this->opEndStatement())) {
    global.fatal << "This is a programming error: ReplaceSsSsXdotsXbySsXdotsX "
    << "called but left expression is not EndStatement." << global.fatal;
  }
  left.theData.children.reserve(left.theData.children.size + right.theData.children.size - 1);
  for (int i = 1; i < right.theData.children.size; i ++) {
    left.theData.addChildOnTop(right.theData[i]);
  }
  left.controlIndex = this->conSequenceStatements();
  (*this->CurrentSyntacticStacK).PopIndexShiftDown((*this->CurrentSyntacticStacK).size - numDots - 1);
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceSsSsXdotsXbySsXdotsX]";
  }
  return true;
}

bool Calculator::ReplaceEXEByCofEE(int theControlIndex) {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  this->DecreaseStackSetCharacterRangeS(2);
  return true;
}

bool Calculator::ReplaceEXEXByEX() {
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  this->DecreaseStackExceptLast(2);
  return true;
}

bool Calculator::ReplaceXXbyO(int theOperation) {
  SyntacticElement& result = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  result.theData.makeAtom(theOperation, *this);
  result.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceXXbyO]";
  }
  return this->DecreaseStackSetCharacterRangeS(1);
}

bool Calculator::ReplaceXXYbyOY(int theOperation) {
  SyntacticElement& result = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  result.theData.makeAtom(theOperation, *this);
  result.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceXXYbyOY]";
  }
  return this->DecreaseStackExceptLast(1);
}


bool Calculator::ReplaceEOEXByEX() {
  SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceEOEXByEX]";
  }
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  middle = *(*this->CurrentSyntacticStacK).lastObject();
  this->DecreaseStackExceptLast(2);
  return true;
}

bool Calculator::ReplaceEXEXBy_OofEE_X(int theOp) {
  SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceEOEXByEX]";
  }
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(theOp);
  newExpr.addChildOnTop(left.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  middle = *(*this->CurrentSyntacticStacK).lastObject();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceEXEXBy_OofEE_X]";
  }
  return this->DecreaseStackExceptLast(2);
}

bool Calculator::ReplaceC1C2Eby_C2ofC1E() {
  SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceC1C2Eby_C2ofC1E]";
  }
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.addChildAtomOnTop(this->GetOperationIndexFromControlIndex(left.controlIndex));
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  return this->DecreaseStackExceptLast(2);
}

bool Calculator::ReplaceXEEXBy_OofEE_X(int inputOperation) {
  SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  SyntacticElement& left  =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  SyntacticElement& right =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(inputOperation);
  newExpr.addChildOnTop(middle.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  middle = *(*this->CurrentSyntacticStacK).lastObject();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceXEEXBy_OofEE_X]";
  }
  return this->DecreaseStackExceptLast(2);
}

bool Calculator::ReplaceXEEBy_OofEE(int inputOperation) {
  SyntacticElement& right =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  SyntacticElement& left  =  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.addChildAtomOnTop(inputOperation);
  newExpr.addChildOnTop(middle.theData);
  newExpr.addChildOnTop(right.theData);
  left.theData = newExpr;
  left.controlIndex = this->conExpression();
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "[Rule: Calculator::ReplaceXEEBy_OofEE]";
  }
  return this->DecreaseStackSetCharacterRangeS(2);
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

bool Calculator::AllowsTensorInPreceding(const std::string& lookAhead) {
  return this->AllowsTimesInPreceding(lookAhead) || lookAhead == "\\otimes";
}

bool Calculator::AllowsTimesInNext(const std::string& thePreceding) {
  if (
    thePreceding == "{}" || thePreceding == "^" || thePreceding == "\\sqrt" || thePreceding == "_"
  ) {
    return false;
  }
  return true;
}

bool Calculator::AllowsTimesInPreceding(const SyntacticElement& thePreceding, const std::string& lookAhead) {
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
  return this->AllowsTimesInPreceding(lookAhead);
}

bool Calculator::AllowsTimesInPreceding(const std::string& lookAhead) {
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

bool Calculator::AllowsPowerInPreceding(const std::string& lookAhead) {
  return lookAhead != "{}";
}

bool Calculator::AllowsPowerInNext(const std::string& lookBehind) {
  return lookBehind != "_";
}

bool Calculator::AllowsLimitProcessInPreceding(const std::string& lookAhead) {
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

bool Calculator::AllowsPlusInPreceding(const std::string& lookAhead) {
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

bool Calculator::AllowsDivideInPreceding(const std::string& lookAhead) {
  return this->AllowsTimesInPreceding(lookAhead);
}

bool Calculator::extractExpressions(Expression& outputExpression, std::string* outputErrors) {
  MacroRegisterFunctionWithName("Calculator::extractExpressions");
  //std::string lookAheadToken;
  std::stringstream errorLog;
  (*this->CurrentSyntacticStacK).reserve((*this->CurrrentSyntacticSouP).size + this->numEmptyTokensStart);
  (*this->CurrentSyntacticStacK).setSize(this->numEmptyTokensStart);
  for (int i = 0; i < this->numEmptyTokensStart; i ++) {
    (*this->CurrentSyntacticStacK)[i] = this->GetEmptySyntacticElement();
  }
  this->parsingLog = "";
  this->NonBoundVariablesInContext.clear();
  this->BoundVariablesInContext.clear();
  const int maxNumTimesOneRuleCanBeCalled = 1000;
  int counterReport = 0;
  int symbolsToIssueReport = 100;
  int minMillisecondsPerReport = 200;
  int64_t lastMilliseconds = global.getElapsedMilliseconds();
  ProgressReport theReport;
  for (
    this->counterInSyntacticSoup = 0;
    this->counterInSyntacticSoup < (*this->CurrrentSyntacticSouP).size;
    this->counterInSyntacticSoup ++
  ) {
    counterReport ++;
    if (counterReport >= symbolsToIssueReport) {
      counterReport = 0;
      int64_t currentMilliseconds = global.getElapsedMilliseconds();
      if (currentMilliseconds - lastMilliseconds > minMillisecondsPerReport) {
        currentMilliseconds = lastMilliseconds;
        std::stringstream reportStream;
        reportStream << "Processed " << this->counterInSyntacticSoup << " out of " << (*this->CurrrentSyntacticSouP).size
        << " syntactic elements. ";
        theReport.report(reportStream.str());
      }
    }
    (*this->CurrentSyntacticStacK).addOnTop((*this->CurrrentSyntacticSouP)[this->counterInSyntacticSoup]);
    int numTimesRulesCanBeAppliedWithoutStackDecrease = 0;
    int minStackSize = this->CurrentSyntacticStacK->size ;
    while (this->ApplyOneRule()) {
      if (this->CurrentSyntacticStacK->size < minStackSize) {
        numTimesRulesCanBeAppliedWithoutStackDecrease = 0;
        minStackSize = this->CurrentSyntacticStacK->size ;
      } else {
        numTimesRulesCanBeAppliedWithoutStackDecrease ++;
      }
      if (numTimesRulesCanBeAppliedWithoutStackDecrease > maxNumTimesOneRuleCanBeCalled) {
        global.fatal << "This may be a programming error: Calculator::ApplyOneRule called more than "
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
  if ((*this->CurrentSyntacticStacK).size == this->numEmptyTokensStart) {
    errorLog << "Non-meaningful/empty input detected (spacebar, enter characters only?).";
  } else if ((*this->CurrentSyntacticStacK).size == this->numEmptyTokensStart + 1) {
    SyntacticElement& result = (*this->CurrentSyntacticStacK)[this->numEmptyTokensStart];
    if (result.errorString == "" && result.controlIndex == this->conExpression()) {
      outputExpression = result.theData;
      success = true;
    } else if (result.errorString != "") {
      errorLog << "Syntax error with message: " << result.errorString;
    } else {
      errorLog << "Syntax error: your command simplifies to a single syntactic element but it is not an expression. <br>";
      errorLog << "It simplifies to:<br> " << this->ToStringSyntacticStackHTMLTable(false);
    }
  } else {
    errorLog << "Syntax error: your command does not simplify to a single syntactic element. <br>";
    errorLog << "Instead it simplifies to:<br> " << this->ToStringSyntacticStackHTMLTable(false);
  }
  if (outputErrors != nullptr) {
    *outputErrors = errorLog.str();
  }
  return success;
}

bool Calculator::ApplyOneRule() {
  MacroRegisterFunctionWithName("Calculator::ApplyOneRule");
  if (this->CurrentSyntacticStacK->size <= this->numEmptyTokensStart) {
    return false;
  }
  const SyntacticElement& lastE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1];
  const std::string& lastS = this->controlSequences[lastE.controlIndex];
  const SyntacticElement& secondToLastE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2];
  const std::string& secondToLastS = this->controlSequences[secondToLastE.controlIndex];
  if (secondToLastS == "Integer" && lastS != "Integer" && lastS != ".") {
    if (this->flagLogSyntaxRules) {
      this->parsingLog += "[Rule: digit to number]";
    }
    return this->ReplaceIntegerXbyEX();
  }
  if (
    lastS == " " &&
    secondToLastS == "\\" &&
    this->CurrentSyntacticStacK->size >= this->numEmptyTokensStart + 2
  ) {
    this->PopTopSyntacticStack();
    if (this->flagLogSyntaxRules) {
      this->parsingLog += "[Rule: remove \\ ]";
    }
    return this->PopTopSyntacticStack();
  }
  if (lastS == " ") {
    if (this->flagLogSyntaxRules) {
      this->parsingLog += "[Rule: remove white space]";
    }
    return this->PopTopSyntacticStack();
  }
  if (lastS == "(" || lastS == "{") {
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1].numNonBoundVariablesInherited = this->NonBoundVariablesInContext.size;
    (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 1].numBoundVariablesInherited = this->BoundVariablesInContext.size;
  }
  const SyntacticElement& thirdToLastE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 3];
  const std::string& thirdToLastS = this->controlSequences[thirdToLastE.controlIndex];
  const SyntacticElement& fourthToLastE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 4];
  const std::string& fourthToLastS = this->controlSequences[fourthToLastE.controlIndex];
  const SyntacticElement& fifthToLastE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 5];
  const std::string& fifthToLastS = this->controlSequences[fifthToLastE.controlIndex];
  const SyntacticElement& sixthToLastE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 6];
  const std::string& sixthToLastS = this->controlSequences[sixthToLastE.controlIndex];
  const SyntacticElement& seventhToLastE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 7];
  const std::string& seventhToLastS = this->controlSequences[seventhToLastE.controlIndex];
  const SyntacticElement& eighthToLastE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 8];
  const std::string& eighthToLastS = this->controlSequences[eighthToLastE.controlIndex];
  if (this->flagLogSyntaxRules) {
    this->parsingLog += "<hr>" + this->ToStringSyntacticStackHTMLTable(false);
  }
  if (secondToLastS == "%" && lastS == "LogParsing") {
    this->parsingLog += "<hr>" + this->ToStringSyntacticStackHTMLTable(false);
    this->flagLogSyntaxRules = true;
    this->parsingLog += "[Rule: remove white space]";
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "LogFull") {
    this->comments
    << "<hr>Requested a full log of the evaluation process.<hr>";
    this->flagLogFullTreeCrunching = true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "UseBracketForIntervals") {
    this->flagUseBracketsForIntervals = true;
    this->comments << "Using brackets for intervals. ";
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "DontUsePredefinedWordSplits") {
    this->flagUsePredefinedWordSplits = false;
    this->comments << "Using predefined word splits -for example xy is replaced by x y. ";
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "WriteLatexDetails") {
    this->flagWriteLatexPlots = true;
    this->comments << "Creating LaTeX files.";
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "PlotShowJavascriptOnly") {
    this->flagPlotShowJavascriptOnly = true;
    this->comments << "Plots show javascript only. ";
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "PlotDetails") {
    this->flagPlotNoControls = false;
    this->comments << "Plot details on. ";
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "HidePolynomialDataStructure") {
    this->flagHidePolynomialBuiltInTypeIndicator = true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "LogEvaluation") {
    this->flagLogEvaluatioN = true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "UseLnInsteadOfLog") {
    this->comments << "Displaying complex logarithms as ln. ";
    this->flagUseLnInsteadOfLog = true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "UseLnAbsInsteadOfLog") {
    this->flagUseLnAbsInsteadOfLogForIntegrationNotation = true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
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
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "LogCache") {
    this->flagLogCache = true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "LogRules") {
    this->flagLogRules = true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "ShowContext") {
    this->flagDisplayContext = true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "NoLogarithmExponentShortcut") {
    this->atomsWhoseExponentsAreInterpretedAsFunctions.
    removeFirstOccurenceSwapWithLast(std::string("\\log"));
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "FullTree") {
    this->flagDisplayFullExpressionTree = true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "HideLHS") {
    this->flagHideLHS = true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS == "%" && lastS == "NoFrac") {
    this->flagUseFracInRationalLaTeX = false;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (this->flagUsePredefinedWordSplits) {
    if (lastS == "Variable") {
      if (this->predefinedWordSplits.contains(this->operations.theKeys[lastE.theData.theData])) {
        return this->ReplaceVbyVdotsVAccordingToPredefinedWordSplits();
      }
    }
  }
  if (secondToLastS == "=" && lastS == ":") {
    return this->ReplaceXXByCon(this->conIsDenotedBy());
  }
  if (secondToLastS == "{" && lastS == "}") {
    return this->ReplaceXXByCon(this->conApplyFunction());
  }
  if (lastS == "\\cdot") {
    return this->ReplaceXByCon(this->conTimes());
  }
  if (lastS == "\\varnothing") {
    return this->ReplaceXByO(this->opEmptySet());
  }
  if (lastS == "\\circ" && secondToLastS != "{" && secondToLastS != "(" && secondToLastS != "^")
    return this->ReplaceXByCon(this->conApplyFunction());
  if (fifthToLastS == "{" && fourthToLastS == "{" && thirdToLastS == "Variable" && secondToLastS == "}" && lastS == "}") {
    return this->ReplaceXXVXdotsXbyE_BOUND_XdotsX(2);
  }
  if (secondToLastS == "Variable" && ((lastS != "}" && lastS != " ") || thirdToLastS != "{" || fourthToLastS != "{")) {
    return this->ReplaceVXdotsXbyE_NONBOUND_XdotsX(1);
  }
  if (thirdToLastS == "Variable" && ((secondToLastS != "}" && secondToLastS != " ") || fourthToLastS != "{" || fifthToLastS != "{")) {
    return this->ReplaceVXdotsXbyE_NONBOUND_XdotsX(2);
  }
  if (fourthToLastS == "{" && thirdToLastS == "Variable" && secondToLastS == "}" && lastS != "}" && lastS != " ") {
    return this->ReplaceVXdotsXbyE_NONBOUND_XdotsX(2);
  }
  if ((secondToLastS == "\\left" || secondToLastS == "\\right") && lastS == "|") {
    return this->ReplaceXYByY();
  }
  if (secondToLastS == "\\left" && (lastS == "(" || lastS == "[" || lastS == "{")) {
    return this->ReplaceXYByY();
  }
  if (secondToLastS == "\\right" && (lastS == ")" || lastS == "]" || lastS == "}")) {
    return this->ReplaceXYByY();
  }
  if (lastS == "=" && secondToLastS == "=") {
    return this->ReplaceXXByCon(this->conEqualEqual());
  }
  if (secondToLastS == "(" && lastS == ")") {
    return this->ReplaceXXByEEmptySequence();
  }
  if (secondToLastS == "\\text" && lastS == "d") {
    if (lastE.theData.isOperationGiven("d")) {
      return this->ReplaceXXbyO(this->opDifferential());
    }
  }
  if (thirdToLastS == "\\text" && secondToLastS == "Expression") {
    if (secondToLastE.theData.isOperationGiven("d")) {
      return this->ReplaceXXYbyOY(this->opDifferential());
    }
  }
  if (thirdToLastS == "Integer" && secondToLastS == "." && lastS == "Integer") {
    return this->ReplaceIntegerDotIntegerByE();
  }
  // there is an ambiguity on how should function application be associated
  // Which is better: x{}y{}z = x{} (y{}z), or x{}y{}z = (x{}y){}z ?
  // In our implementation, we choose x{}y{}z= x{} (y{}z). Although this is slightly harder to implement,
  // it appears to be the more natural choice.
  // if (fourthToLastS == "Expression" && thirdToLastS == "{}" && secondToLastS == "Expression"
  //     && this->AllowsApplyFunctionInPreceding(lastS) )
  //   return this->ReplaceEOEXByEX(secondToLastE.theData.format);
  if (
    fourthToLastS == "Expression" &&
    thirdToLastS == "{}" &&
    secondToLastS == "Expression" &&
    this->AllowsApplyFunctionInPreceding(lastS)
  ) {
    return this->ReplaceEXEXByEX();
  }
  //end of ambiguity.
  if (
    fifthToLastS == "\\lim" &&
    fourthToLastS == "_" &&
    thirdToLastS == "Expression" &&
    secondToLastS == "Expression" &&
    this->AllowsLimitProcessInPreceding(lastS)
  ) {
    return this->ReplaceOXEEXByEX();
  }
  if (
    fourthToLastS == "Expression" &&
    thirdToLastS == "_" &&
    secondToLastS == "Expression" &&
    lastS != "_"
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (fourthToLastS == "{"  && thirdToLastS == "{}" && secondToLastS == "Expression" && lastS == "}") {
    return this->ReplaceXYYXByYY();
  }
  if (secondToLastS == "Expression" && lastS == "!") {
    return this->ReplaceEOByE();
  }
  if (
    thirdToLastS == "\\binom" &&
    secondToLastS == "Expression" &&
    lastS == "Expression"
  ) {
    return this->ReplaceXEEBy_OofEE(this->opBinom());
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "^" &&
    fourthToLastS == "Expression" &&
    this->AllowsPowerInPreceding(lastS) &&
    this->AllowsPowerInNext(fifthToLastS)
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "\\setminus" &&
    fourthToLastS == "Expression" &&
    this->AllowsPowerInPreceding(lastS)
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "or" &&
    fourthToLastS == "Expression" &&
    this->allowsOrInPreceding(lastS)
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "and" &&
    fourthToLastS == "Expression" &&
    this->allowsAndInPreceding(lastS)
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "+" &&
    fourthToLastS == "Expression" &&
    this->AllowsPlusInPreceding(lastS)
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "\\oplus" &&
    fourthToLastS == "Expression" &&
    this->AllowsPlusInPreceding(lastS)
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    fifthToLastS == "|" && secondToLastS == "Expression" &&
    thirdToLastS == "+" && fourthToLastS == "Expression" && lastS == "|"
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" && thirdToLastS == "-" &&
    fourthToLastS == "Expression" && this->AllowsPlusInPreceding(lastS)
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" && thirdToLastS == "-" &&
    fourthToLastS == "Expression" && lastS == "|" && fifthToLastS == "|"
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" && thirdToLastS == "-" && this->AllowsPlusInPreceding(lastS)
  ) {
    return this->ReplaceOEXByEX();
  }
  if (secondToLastS == "Expression" && thirdToLastS == "-" && lastS == "|" && fourthToLastS == "|") {
    return this->ReplaceOEXByEX();
  }
  if (secondToLastS == "Expression" && thirdToLastS == "mod" && fourthToLastS == "Expression") {
    return this->ReplaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "\\in" &&
    fourthToLastS == "Expression" &&
    this->allowsInInPreceding(lastS)
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (secondToLastS == "Expression" && thirdToLastS == "\\choose" && fourthToLastS == "Expression") {
    return this->ReplaceEXEXBy_OofEE_X(this->opBinom());
  }
  if (secondToLastS == "Expression" && thirdToLastS == "\\otimes" && fourthToLastS == "Expression" && this->AllowsTensorInPreceding(lastS)) {
    return this->ReplaceEOEXByEX();
  }
  if (secondToLastS == "Expression" && thirdToLastS == "Expression" && fourthToLastS == "\\frac") {
    return this->ReplaceXEEXBy_OofEE_X(this->opDivide());
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "\\times" &&
    fourthToLastS == "Expression" &&
    this->AllowsTimesInPreceding(lastS)
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "*" &&
    fourthToLastS == "Expression" &&
    this->AllowsTimesInPreceding(secondToLastE, lastS)) {
    return this->ReplaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "/" &&
    fourthToLastS == "Expression" &&
    this->AllowsDivideInPreceding(lastS)
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    secondToLastS == "Expression" &&
    thirdToLastS == "\\to" &&
    fourthToLastS == "Expression" &&
    this->AllowsLimitProcessInPreceding(lastS)
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    this->atomsWhoseExponentsAreInterpretedAsFunctions.contains(fifthToLastS) && fourthToLastS == "^" &&
    thirdToLastS == "Expression" && secondToLastS == "Expression" &&
    this->AllowsTimesInPreceding(lastS)
  ) {
    return this->ReplaceOOEEXbyEXpowerLike();
  }
  if (secondToLastS == "\\int" && lastS != "_") {
    return this->ReplaceOXbyEX();
  }
  if (thirdToLastS == "\\int" && secondToLastS == "_" && lastS == "Expression") {
    return this->ReplaCeOXdotsXbyEXdotsX(2);
  }
  if (
    fourthToLastS == "\\int" && thirdToLastS == "_" &&
    secondToLastS == "Expression" && (lastS == "^" || lastS == "EndProgram")
  ) {
    return this->ReplaCeOXdotsXbyEXdotsX(3);
  }
  if (
    fifthToLastS == "\\int" && fourthToLastS == "_" && thirdToLastS == "{}" &&
    secondToLastS == "^" && lastS == "{}"
  ) {
    this->PopTopSyntacticStack();
    this->PopTopSyntacticStack();
    this->PopTopSyntacticStack();
    this->ReplaceOXbyEX();
    return this->PopTopSyntacticStack();
  }
  if (
    fifthToLastS == "LogBase" && fourthToLastS == "_" && thirdToLastS == "Expression" &&
    secondToLastS == "Expression" && this->AllowsTimesInPreceding(lastS)
  ) {
    return this->ReplaceOXEEXByEX();
  }
  if (
    seventhToLastS == "\\sqrt" && sixthToLastS == "[" && fifthToLastS == "Expression" &&
    fourthToLastS == "]" && thirdToLastS == "Expression"
  ) {
    return this->ReplaceOXEXEXXByEXX();
  }
  if (
    this->AllowsTimesInNext(fourthToLastS) &&
    thirdToLastS == "Expression" &&
    secondToLastS == "Expression" &&
    this->AllowsTimesInPreceding(secondToLastE, lastS)
  ) {
    this->ReplaceEEXBy_CofEE_X(this->conTimes());
    Expression impliedFunApplication;
    if (this->outerTimesToFunctionApplication(
      *this,
      (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2].theData,
      impliedFunApplication
    )) {
      (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size - 2].theData = impliedFunApplication;
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
    return this->ReplaceXXByCon(this->conGEQ());
  }
  if (thirdToLastS == "|" && secondToLastS == "Expression" && lastS == "|") {
    return this->ReplaceOEXByE();
  }
  if (thirdToLastS == "(" && secondToLastS == "Expression" && lastS == ")") {
    return this->ReplaceXEXByE();
  }
  if (thirdToLastS == "{" && secondToLastS == "Expression" && lastS == "}") {
    return this->ReplaceXEXByE();
  }
  if (thirdToLastS == "(" && secondToLastS == "\\circ" && lastS == ")") {
    return this->ReplaceOXbyEX();
  }
  if (thirdToLastS == "{" && secondToLastS == "\\circ" && lastS == "}") {
    return this->ReplaceOXbyEX();
  }
  if (secondToLastS == "^" && lastS == "\\circ") {
    return this->ReplaCeObyE();
  }
  if (this->flagUseBracketsForIntervals) {
    if (thirdToLastS == "(" && secondToLastS == "Expression" && lastS == "]") {
      return this->ReplaceXEXByEcontainingOE(this->opIntervalRightClosed());
    }
    if (thirdToLastS == "[" && secondToLastS == "Expression" && lastS == ")") {
      return this->ReplaceXEXByEcontainingOE(this->opIntervalLeftClosed());
    }
    if (thirdToLastS == "[" && secondToLastS == "Expression" && lastS == "]") {
      return this->ReplaceXEXByEcontainingOE(this->opIntervalClosed());
    }
    if (this->flagUseBracketsForIntervals) {
      if (secondToLastE.theData.isSequenceNElements(2)) {
        if (
          this->isSeparatorFromTheLeftForInterval(fourthToLastS) &&
          thirdToLastS == "(" &&
          secondToLastS == "Sequence" &&
          lastS == ")"
        ) {
          return this->ReplaceXEXByEcontainingOE(this->opIntervalOpen());
        }
      }
    }
  }
  if (thirdToLastS == "\"" && secondToLastS == "Expression" && lastS == "\"") {
    return this->ReplaceXEXByEcontainingOE(this->opQuote());
  }
  if (secondToLastS == "\"" && lastS == "\"") {
    return this->ReplaceXXByEmptyString();
  }
  if (lastS == "Expression" && secondToLastS == "~" && thirdToLastS == "Expression") {
    return this->ReplaceEOEByE();
  }
  if (secondToLastS == "==" && lastS == "=") {
    this->ReplaceXXByCon(this->conEqualEqualEqual());
  }
  if (
    this->isSeparatorFromTheRightGeneral(lastS) &&
    secondToLastS == "Expression" &&
    thirdToLastS == "===" &&
    fourthToLastS == "Expression"
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastS) &&
    this->isSeparatorFromTheRightForDefinition(lastS) &&
    secondToLastS == "Expression" &&
    thirdToLastS == "==" &&
    fourthToLastS == "Expression"
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastS) &&
    this->isSeparatorFromTheRightForDefinition(lastS) &&
    secondToLastS == "Expression" &&
    thirdToLastS == "\\geq" &&
    fourthToLastS == "Expression"
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && this->isSeparatorFromTheRightForDefinition(lastS) && secondToLastS == "Expression" && thirdToLastS == "\\leq" && fourthToLastS == "Expression") {
    return this->ReplaceEOEXByEX();
  }
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && this->isSeparatorFromTheRightForDefinition(lastS) && secondToLastS == "Expression" && thirdToLastS == ">" && fourthToLastS == "Expression") {
    return this->ReplaceEOEXByEX();
  }
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && this->isSeparatorFromTheRightForDefinition(lastS) && secondToLastS == "Expression" && thirdToLastS == "<" && fourthToLastS == "Expression") {
    return this->ReplaceEOEXByEX();
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastS) &&
    fourthToLastS == "Expression" &&
    thirdToLastS == "=" &&
    secondToLastS == "Expression" &&
    this->isSeparatorFromTheRightForDefinition(lastS)
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastS) &&
    fourthToLastS == "Expression" &&
    thirdToLastS == "=:" &&
    secondToLastS == "Expression" &&
    this->isSeparatorFromTheRightForDefinition(lastS)
  ) {
    return this->ReplaceEOEXByEX();
  }
  if (
    lastS == "Sequence" &&
    lastE.theData.size() == 0 &&
    lastE.theData.theData == this->opLisT()
  ) {
    return this->ReplaceXByCon(this->controlSequences.getIndexNoFail("MakeSequence"));
  }
  if (thirdToLastS == "\\sqrt" && secondToLastS == "Expression") {
    return this->ReplaceSqrtEXByEX();
  }
  if (fourthToLastS == "\\sqrt" && thirdToLastS == "Expression") {
    return this->ReplaceSqrtEXXByEXX();
  }
  if (fourthToLastS == "\\sqrt" && thirdToLastS == "{}" && secondToLastS == "Expression") {
    return this->ReplaceSqrtXEXByEX();
  }
  if (
    sixthToLastS == "\\sqrt" && fifthToLastS == "[" && fourthToLastS == "Expression" &&
    thirdToLastS == "]" && secondToLastS == "Expression"
  ) {
    return this->ReplaceOXEXEXByEX();
  }
  if (
    this->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(thirdToLastS) &&
    secondToLastS == "Expression" && this->AllowsTimesInPreceding(lastS)
  ) {
    return this->ReplaceOEXByEX();
  }
  if (
    this->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(fourthToLastS) &&
    thirdToLastS == "^" && secondToLastS == "Expression" && this->isSeparatorFromTheRightGeneral(lastS)
  ) {
    return this->ReplaceC1C2Eby_C2ofC1E();
  }
  if (
    this->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(fourthToLastS) &&
    thirdToLastS == "{}" && secondToLastS == "Expression" && this->AllowsApplyFunctionInPreceding(lastS)
  ) {
    return this->ReplaceOXEXByEX();
  }
  if (
    this->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(secondToLastS) &&
    this->isSeparatorFromTheRightGeneral(lastS)
  ) {
    return this->ReplaceOXbyEX();
  }
  if (fourthToLastS == "\\log" && thirdToLastS == "_" && secondToLastS == "Expression") {
    return this->ReplaceXYYYByConYYY(this->controlSequences.getIndexNoFail("LogBase"));
  }
  //Some synonyms:
  if (lastS == "ln" || lastS == "log" || lastS == "\\ln") {
    return this->ReplaceXByCon(this->controlSequences.getIndexNoFail("\\log"));
  }
  if (lastS == "infinity" || lastS == "infty") {
    return this->ReplaceXByO(this->opInfinity());
  }
  if (lastS == "pi") {
    return this->ReplaceXByO(this->opPi());
  }
  if (lastS == "arcsin") {
    return this->ReplaceXByCon(this->controlSequences.getIndexNoFail("\\arcsin"));
  }
  if (lastS == "arccos") {
    return this->ReplaceXByCon(this->controlSequences.getIndexNoFail("\\arccos"));
  }
  if (lastS == "arctan") {
    return this->ReplaceXByCon(this->controlSequences.getIndexNoFail("\\arctan"));
  }
  if (lastS == "sin") {
    return this->ReplaceXByCon(this->controlSequences.getIndexNoFail("\\sin"));
  }
  if (lastS == "cos") {
    return this->ReplaceXByCon(this->controlSequences.getIndexNoFail("\\cos"));
  }
  if (lastS == "tan") {
    return this->ReplaceXByCon(this->controlSequences.getIndexNoFail("\\tan"));
  }
  if (lastS == "cot") {
    return this->ReplaceXByCon(this->controlSequences.getIndexNoFail("\\cot"));
  }
  if (lastS == "csc") {
    return this->ReplaceXByCon(this->controlSequences.getIndexNoFail("\\csc"));
  }
  if (lastS == "sec") {
    return this->ReplaceXByCon(this->controlSequences.getIndexNoFail("\\sec"));
  }
  if (lastS == "sqrt") {
    return this->ReplaceXByCon(this->controlSequences.getIndexNoFail("\\sqrt"));
  }
  //end of some synonyms
  if (fourthToLastS == "Expression" && thirdToLastS == "\\cup" && secondToLastS == "Expression" && this->AllowsTimesInPreceding(lastS)) {
    return this->ReplaceEOEXByEX();
  }
  if (fourthToLastS == "Expression" && thirdToLastS == "\\cap" && secondToLastS == "Expression" && this->AllowsTimesInPreceding(lastS)) {
    return this->ReplaceEOEXByEX();
  }
  if (fourthToLastS == "Expression" && thirdToLastS == "\\sqcup" && secondToLastS == "Expression" && this->AllowsTimesInPreceding(lastS)) {
    return this->ReplaceEOEXByEX();
  }
  if (thirdToLastS == "if" && secondToLastS == "Expression" && this->allowsIfInPreceding(lastS)) {
    return this->ReplaceOXXByEXX();
  }
  if (
    fourthToLastS == "Sequence" && thirdToLastS == "," &&
    secondToLastS == "Expression" && (
      lastS == "," || lastS == ")" || lastS == "}" ||
      (this->flagUseBracketsForIntervals && lastS == "]")
    )
  ) {
    return this->ReplaceSequenceUXEYBySequenceZY(this->conSequence());
  }
  if (
    (thirdToLastS != "[" || this->flagUseBracketsForIntervals)&&
    secondToLastS == "Expression" && lastS == ","
  ) {
    return this->ReplaceYXBySequenceX(this->conSequence());
  }
  if (thirdToLastS == "MakeSequence" && secondToLastS == "{}" && lastS == "Expression") {
    return this->ReplaceXXYBySequenceY(this->conExpression());
  }
  if (fourthToLastS == "MakeSequence" && thirdToLastS == "{}" && secondToLastS == "Expression") {
    return this->ReplaceXXYXBySequenceYX(this->conExpression());
  }
  if (
    fifthToLastS == "Expression" && fourthToLastS == "{}" &&
    thirdToLastS == "(" &&
    secondToLastS == "Sequence" && lastS == ")"
  ) {
    return this->ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  }
  if (
    fifthToLastS == "Expression" && fourthToLastS == "{}" &&
    thirdToLastS == "{" &&
    secondToLastS == "Sequence" && lastS == "}"
  ) {
    return this->ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
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
    return this->ReplaceXXbyEX();
  }
  if (
    fourthToLastS == "\\begin" &&
    thirdToLastS == "{" &&
    secondToLastS == "pmatrix" &&
    lastS == "}"
  ) {
    return this->ReplaceXdotsXByMatrixStart(4);
  }
  if (
    fifthToLastS == "\\begin" && fourthToLastS == "{" &&
    thirdToLastS == "array" && secondToLastS == "}" && lastS == "Expression"
  ) {
    return this->ReplaceXdotsXByMatrixStart(5);
  }
  if (
    fourthToLastS == "\\end" && thirdToLastS == "{" &&
    (secondToLastS == "array"  || secondToLastS == "pmatrix") &&
    lastS == "}"
  ) {
    return this->ReplaceXXXXByCon(this->conMatrixEnd());
  }
  if (lastS == "\\" && secondToLastS == "\\") {
    return this->ReplaceXXByCon(this->controlSequences.getIndexNoFail("\\\\"));
  }
  if (lastS == "\\displaystyle") {
    return this->PopTopSyntacticStack();
  }
  if (
    thirdToLastS == "Matrix" && secondToLastS == "Expression" &&
    lastS == "&"
  ) {
    return this->ReplaceMatrixEXByMatrix();
  }
  if (
    thirdToLastS == "Matrix" && secondToLastS == "Expression" &&
    lastS == "MatrixEnd"
  ) {
    return this->ReplaceMatrixEXByMatrixX();
  }
  if (
    thirdToLastS == "Matrix" && secondToLastS == "Expression" &&
    lastS == "\\\\"
  ) {
    return this->ReplaceMatrixEXByMatrixNewRow();
  }
  if (secondToLastS == "Matrix" && lastS == "MatrixEnd") {
    return this->ReplaceMatrixXByE();
  }
  if (!this->flagUseBracketsForIntervals) {
    if (
      fifthToLastS == "[" && fourthToLastS == "Expression" &&
      thirdToLastS == "," &&
      secondToLastS == "Expression" && lastS == "]"
    ) {
      return this->ReplaceXEXEXBy_CofEE(this->conLieBracket());
    }
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(eighthToLastS) &&
    seventhToLastS == "Expression" &&
    sixthToLastS == ":" && fifthToLastS == "if" && fourthToLastS == "Expression" &&
    thirdToLastS == "=" && secondToLastS == "Expression" &&
    this->isSeparatorFromTheRightForDefinition(lastS)
  ) {
    return this->ReplaceEXXEXEXBy_CofEEE_X(this->conDefineConditional());
  }
  if (thirdToLastS == "SequenceStatements" && secondToLastS == "Expression" && (lastS == ")" || lastS == "}")) {
    return this->ReplaceEXdotsXbySsXdotsX(1);
  }
  if (secondToLastS == "Expression" && lastS == ";") {
    return this->ReplaceEXdotsXBySs(1);
  }
  if (secondToLastS == "Expression" && lastS == "EndProgram") {
    return this->ReplaceEXdotsXbySsXdotsX(1);
  }
  if (
    (thirdToLastS == "(" || thirdToLastS == "{" ) &&
    secondToLastS == "SequenceStatements" &&
    (lastS == ")" || lastS == "}")
  ) {
    return this->ReplaceXEXByE();
  }
  if (secondToLastS == "SequenceStatements" && lastS == "SequenceStatements") {
    return this->ReplaceSsSsXdotsXbySsXdotsX(0);
  }
  if (
    thirdToLastS == "SequenceStatements" &&
    secondToLastS == "SequenceStatements" &&
    (lastS == "EndProgram" || lastS == ")" || lastS == "}")
  ) {
    return this->ReplaceSsSsXdotsXbySsXdotsX(1);
  }
  if (secondToLastS == "SequenceStatements" && lastS == "EndProgram") {
    return this->ReplaceXXByCon(this->conExpression());
  }
  if (lastS == "EndProgram") {
    return this->DecreaseStackSetCharacterRangeS(1);
  }
  return false;
}

