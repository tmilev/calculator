//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "string_constants.h"
ProjectInformationInstance ProjectInfoVpf6_05cpp(__FILE__, "Calculator core evaluation engine. ");

std::string Calculator::ToStringFunctionHandlersJSON() {
  MacroRegisterFunctionWithName("Calculator::ToStringFunctionHandlersJSON");
  JSData output;
  output.theType = JSData::token::tokenObject;
  for (int currentAtomIndex = 0; currentAtomIndex < this->theAtoms.size; currentAtomIndex ++) {
    const std::string& currentAtom = this->theAtoms[currentAtomIndex];
    JSData currentAtomJSON, currentFunctionListDirect, currentFunctionListComposite;
    currentAtomJSON.theType = JSData::token::tokenObject;
    currentFunctionListDirect.theType = JSData::token::tokenArray;
    currentFunctionListComposite.theType = JSData::token::tokenArray;
    if (currentAtomIndex >= this->FunctionHandlers.size) {
      crash << "This shouldn't happen: bad atom index. " << crash;
    }
    if (this->FunctionHandlers[currentAtomIndex].size > 0) {
      for (int j = 0; j < this->FunctionHandlers[currentAtomIndex].size; j ++) {
        if (this->FunctionHandlers[currentAtomIndex][j].flagIamVisible) {
          currentFunctionListDirect[j] = this->FunctionHandlers[currentAtomIndex][j].ToJSON();
        }
      }
    }
    int indexCompositeHander = this->operationsComposite.GetIndex(currentAtom);
    if (indexCompositeHander != - 1) {
      for (int j = 0; j < this->operationsCompositeHandlers[indexCompositeHander].size; j ++) {
        if (this->operationsCompositeHandlers[indexCompositeHander][j].flagIamVisible) {
          currentFunctionListComposite[j] = this->operationsCompositeHandlers[indexCompositeHander][j].ToJSON();
        }
      }
    }
    currentAtomJSON["regular"] = currentFunctionListDirect;
    currentAtomJSON["composite"] = currentFunctionListComposite;
    output[currentAtom] = currentAtomJSON;
  }
  return output.ToString(false);
}

const Expression& Calculator::EZero() {
  if (this->frequentConstantZero.owner == nullptr) {
    this->frequentConstantZero.AssignValue(0, *this);
  }
  return this->frequentConstantZero;
}

const Expression& Calculator::EOne() {
  if (this->frequentConstantOne.owner == nullptr) {
    this->frequentConstantOne.AssignValue(1, *this);
  }
  return this->frequentConstantOne;
}

const Expression& Calculator::EMOne() {
  if (this->frequentConstantMinusOne.owner == nullptr) {
    this->frequentConstantMinusOne.AssignValue(- 1, *this);
  }
  return this->frequentConstantMinusOne;
}

const Expression& Calculator::EFour() {
  if (this->frequentConstantFour.owner == nullptr) {
    this->frequentConstantFour.AssignValue(4, *this);
  }
  return this->frequentConstantFour;
}

const Expression& Calculator::ETwo() {
  if (this->frequentConstantTwo.owner == nullptr) {
    this->frequentConstantTwo.AssignValue(2, *this);
  }
  return this->frequentConstantTwo;
}

const Expression& Calculator::EMHalf() {
  if (this->frequentConstantMinusHalf.owner == nullptr) {
    this->frequentConstantMinusHalf.AssignValue(Rational(- 1, 2), *this);
  }
  return this->frequentConstantMinusHalf;
}

const Expression& Calculator::EInfinity() {
  if (this->frequentEInfinity.owner == nullptr) {
    this->frequentEInfinity.MakeAtom(this->opInfinity(), *this);
  }
  return this->frequentEInfinity;
}

const Expression& Calculator::EMInfinity() {
  if (this->frequentEMInfinity.owner == nullptr) {
    this->frequentEMInfinity = this->EMOne() * this->EInfinity();
  }
  return this->frequentEMInfinity;
}

void Calculator::DoLogEvaluationIfNeedBe(Function& inputF) {
  if (this->historyStack.size > 0) {
    this->historyRuleNames.LastObject().LastObject() = inputF.calculatorIdentifier;
  }
  if (!this->flagLogEvaluatioN) {
    return;
  }
  *this << "<hr>Built-in substitution: " << inputF.ToStringSummary()
  << "<br>" << theGlobalVariables.GetElapsedSeconds()-this->LastLogEvaluationTime
  << " second(s) since last log entry. "
  << "Rule stack id: "
  << this->RuleStackCacheIndex << ", stack size: " << this->RuleStack.size();
  this->LastLogEvaluationTime = theGlobalVariables.GetElapsedSeconds();
}

bool Calculator::outerStandardFunction(
  Calculator& theCommands, const Expression& input, Expression& output, int opIndexParentIfAvailable
) {
  MacroRegisterFunctionWithName("Calculator::outerStandardFunction");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsLisT()) {
    return false;
  }
  const Expression& functionNameNode = input[0];
  if (functionNameNode.StartsWith()) {
    const List<Function>* theHandlers = theCommands.GetOperationCompositeHandlers(functionNameNode[0].theData);
    if (theHandlers != 0) {
      for (int i = 0; i < theHandlers->size; i ++) {
        if ((*theHandlers)[i].ShouldBeApplied(opIndexParentIfAvailable)) {
          if ((*theHandlers)[i].theFunction(theCommands, input, output)) {
            theCommands.DoLogEvaluationIfNeedBe((*theHandlers)[i]);
            return true;
          }
        }
      }
    }
  }
  if (!functionNameNode.IsAtom()) {
    return false;
  }
  for (int i = 0; i < theCommands.FunctionHandlers[functionNameNode.theData].size; i ++) {
    if (!theCommands.FunctionHandlers[functionNameNode.theData][i].flagIsInner) {
      Function& outerFun = theCommands.FunctionHandlers[functionNameNode.theData][i];
      if (!outerFun.ShouldBeApplied(opIndexParentIfAvailable)) {
        continue;
      }
      if (outerFun.theFunction(theCommands, input, output)) {
        if (output != input) {
          output.CheckConsistency();
          theCommands.DoLogEvaluationIfNeedBe(outerFun);
          return true;
        }
      }
    } else {
      Function& innerFun = theCommands.FunctionHandlers[functionNameNode.theData][i];
      if (!innerFun.ShouldBeApplied(opIndexParentIfAvailable)) {
        continue;
      }
      if (input.children.size > 2) {
        if (innerFun.inputFitsMyInnerType(input)) {
          if (innerFun.theFunction(theCommands, input, output)) {
            output.CheckConsistency();
            theCommands.DoLogEvaluationIfNeedBe(innerFun);
            return true;
          }
        }
      } else if (input.children.size == 2) {
        if (innerFun.inputFitsMyInnerType(input[1])) {
          if (innerFun.theFunction(theCommands, input[1], output)) {
            output.CheckConsistency();
            theCommands.DoLogEvaluationIfNeedBe(innerFun);
            return true;
          }
        }
      }
    }
  }
  return false;
}

bool Calculator::ExpressionMatchesPattern(
  const Expression& thePattern, const Expression& input, MapList<Expression, Expression>& matchedExpressions,
  std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("Calculator::ExpressionMatchesPattern");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (!(thePattern.owner == this && input.owner == this)) {
    crash << "This is a programming error. "
    << "Either a pattern or an input has a wrongly  initialized owner: the pattern is "
    << thePattern.ToString() << " and the input is "
    << input.ToString() << ". The error is certainly in the preceding code; here "
    << "is a stack trace, however beware that the error might be in code preceding the stack loading. "
    << crash;
  }
  if (commentsGeneral!= 0) {
    *commentsGeneral << " <hr> current input: " << input.ToString() << "<br>current pattern: " << thePattern.ToString();
    *commentsGeneral << "<br> current matched expressions: " << matchedExpressions.ToStringHtml();
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (this->RecursionDeptH>this->MaxRecursionDeptH) {
    std::stringstream out;
    out << "Max recursion depth of " << this->MaxRecursionDeptH << " exceeded whlie trying to match expression pattern "
    << thePattern.ToString() << " onto expression " << input.ToString();
    this->evaluationErrors.AddOnTop(out.str());
    return false;
  }
//  if (this->opDefine() == input.theOperation)
//    return false;
  int opVarB = this->opBind();
  if (thePattern.IsListStartingWithAtom(opVarB)) {
    if (!matchedExpressions.Contains(thePattern)) {
      matchedExpressions.SetKeyValue(thePattern, input);
    }
    if (matchedExpressions.GetValueCreate(thePattern) != input) {
      return false;
    }
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "<br><b>Match!</b>";
    }
    return true;
  }
  if (thePattern.theData != input.theData || thePattern.size() != input.size()) {
    return false;
  }
  bool isGoodRegularOrder = true;
  int numMatchedExpressionsAtStart = matchedExpressions.size();
  for (int i = 0; i < thePattern.size(); i ++) {
    if (!(this->ExpressionMatchesPattern(thePattern[i], input[i], matchedExpressions, commentsGeneral))) {
      if (i == 0) {
        return false;
      }
      isGoodRegularOrder = false;
      break;
    }
  }

  if (!isGoodRegularOrder) {
    if (!input.StartsWith(thePattern.owner->opPlus())) {
      return false;
    }
    matchedExpressions.theValues.SetSize(numMatchedExpressionsAtStart);
    matchedExpressions.theKeys.SetSize(numMatchedExpressionsAtStart);
    for (int i = 1; i < thePattern.size(); i ++) {
      if (!(this->ExpressionMatchesPattern(
        thePattern[i], input[thePattern.size() - i], matchedExpressions, commentsGeneral
      ))) {
        return false;
      }
    }
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<br><b>Match!</b>";
  }
  return true;
}

void StateMaintainerCalculator::AddRule(const Expression& theRule) {
  if (this->owner == nullptr) {
    crash << "StackMaintainerCalculator has zero owner. " << crash;
  }
  this->owner->RuleStack.AddChildOnTop(theRule);
  std::string currentRule;
  if (
    theRule.StartsWith(this->owner->opRulesOn()) ||
    theRule.StartsWith(this->owner->opRulesOff())
  ) {
    for (int j = 1; j < theRule.size(); j ++) {
      if (!theRule[j].IsOfType(&currentRule)) {
        continue;
      }
      if (!this->owner->namedRules.Contains(currentRule)) {
        continue;
      }
      this->owner->GetFunctionHandlerFromNamedRule(currentRule).flagDisabledByUser =
      theRule.StartsWith(this->owner->opRulesOff());
    }
  }
  this->owner->RuleStackCacheIndex = this->owner->cachedRuleStacks.GetIndex(this->owner->RuleStack);
  if (this->owner->RuleStackCacheIndex == - 1) {
    if (this->owner->cachedRuleStacks.size< this->owner->MaxCachedExpressionPerRuleStack) {
      this->owner->RuleStackCacheIndex = this->owner->cachedRuleStacks.size;
      this->owner->cachedRuleStacks.AddOnTop(this->owner->RuleStack);
    }
  }
  if (this->owner->flagLogRules) {
    *this->owner << "<hr>Added rule " << theRule.ToString() << " with state identifier "
    << this->owner->RuleStackCacheIndex;
  }
}

Expression& StateMaintainerCalculator::GetCurrentHistory() {
  MacroRegisterFunctionWithName("StateMaintainerCalculator::GetCurrentHistory");
  return this->owner->historyStack[this->historyOuterSize- 1][this->historyMiddleSize- 1];
}

std::string& StateMaintainerCalculator::GetCurrentHistoryRuleNames() {
  MacroRegisterFunctionWithName("StateMaintainerCalculator::GetCurrentHistoryRuleNames");
  return this->owner->historyRuleNames[this->historyOuterSize - 1][this->historyMiddleSize - 1];
}

StateMaintainerCalculator::StateMaintainerCalculator(Calculator& inputBoss) {
  this->owner = &inputBoss;
  this->historyOuterSize = inputBoss.historyStack.size;
  this->historyMiddleSize = - 1;
  if (inputBoss.historyStack.size > 0) {
    this->historyMiddleSize = inputBoss.historyStack.LastObject().size;
  }
  this->startingRuleStackIndex = inputBoss.RuleStackCacheIndex;
  this->startingRuleStackSize = inputBoss.RuleStack.size();
}

StateMaintainerCalculator::~StateMaintainerCalculator() {
  if (this->owner == nullptr) {
    return;
  }
  this->owner->historyStack.SetSize(this->historyOuterSize);
  this->owner->historyRuleNames.SetSize(this->historyOuterSize);
  if (this->owner->historyStack.size > 0) {
    this->owner->historyStack.LastObject().SetSize(this->historyMiddleSize);
    this->owner->historyRuleNames.LastObject().SetSize(this->historyMiddleSize);
  }
  Expression& theRuleStack = this->owner->RuleStack;
  std::string currentRuleName;
  bool shouldUpdateRules = false;
  for (int i = this->startingRuleStackSize; i < theRuleStack.size(); i ++) {
    if (
      theRuleStack[i].StartsWith(this->owner->opRulesOn()) ||
      theRuleStack[i].StartsWith(this->owner->opRulesOff())
    ) {
      for (int j = 1; j < theRuleStack[i].size(); j ++) {
        if (!theRuleStack[i][j].IsOfType<std::string>(&currentRuleName)) {
          continue;
        }
        if (!this->owner->namedRules.Contains(currentRuleName)) {
          continue;
        }
        Function& currentRule = this->owner->GetFunctionHandlerFromNamedRule(currentRuleName);
        currentRule.flagDisabledByUser = currentRule.flagDisabledByUserDefaultValue;
        //stOutput << "<br>DEBUG: resetting rule: " << currentRule.calculatorIdentifier
        //<< " to value: "
        //<< currentRule.flagDisabledByUserDefaultValue
        //<< " with startingrulestacksize: "
        //<< this->startingRuleStackSize;
        shouldUpdateRules = true;
      }
    }
  }
  if (shouldUpdateRules) {
    for (int i = 0; i < this->startingRuleStackSize; i ++) {
      if (theRuleStack[i].StartsWith(this->owner->opRulesOn())) {
        for (int j = 1; j < theRuleStack[i].size(); j ++) {
          Function& currentFun = this->owner->GetFunctionHandlerFromNamedRule(
            theRuleStack[i][j].GetValue<std::string>()
          );
          currentFun.flagDisabledByUser = false;
        }
      } else if (theRuleStack[i].StartsWith(this->owner->opRulesOff())) {
        for (int j = 1; j < theRuleStack[i].size(); j ++) {
          Function& currentFun = this->owner->GetFunctionHandlerFromNamedRule(
            theRuleStack[i][j].GetValue<std::string>()
          );
          currentFun.flagDisabledByUser = true;
        }
      }
    }
  }
  this->owner->RuleStackCacheIndex = this->startingRuleStackIndex;
  this->owner->RuleStack.children.SetSize(this->startingRuleStackSize);
  this->owner = nullptr;
}

Expression Calculator::GetNewBoundVar() {
  Expression result(*this);
  result.AddChildAtomOnTop(this->opBind());
  result.AddChildOnTop(this->GetNewAtom());
  return result;
}

Expression Calculator::GetNewAtom() {
  std::string atomPrefix;
  std::string candidate;
  while (true) {
    for (char a = 'a'; a <= 'z'; a ++) {
      candidate = atomPrefix;
      candidate.push_back(a);
      if (!this->theAtoms.Contains(candidate)) {
        Expression result;
        result.MakeAtom(candidate, *this);
        return result;
      }
    }
    atomPrefix = candidate;
  }
}

bool Calculator::AccountRule(const Expression& ruleE, StateMaintainerCalculator& theRuleStackMaintainer) {
  MacroRegisterFunctionWithName("Calculator::AccountRule");
  RecursionDepthCounter theRecursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH > this->MaxRecursionDeptH) {
    return false;
  }
  if (ruleE.IsCalculatorStatusChanger()) {
    theRuleStackMaintainer.AddRule(ruleE);
  }
  if (!ruleE.IsListStartingWithAtom(this->opCommandEnclosure())) {
    return true;
  }
  if (ruleE.size() <= 1) {
    return true;
  }
  if (!ruleE[1].StartsWith(this->opEndStatement())) {
    return this->AccountRule(ruleE[1], theRuleStackMaintainer);
  }
  for (int i = 1; i < ruleE[1].size(); i ++) {
    if (!this->AccountRule(ruleE[1][i], theRuleStackMaintainer)) {
      return false;
    }
  }
  return true;
}

bool Calculator::EvaluateExpression(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::EvaluateExpression");
  bool notUsed = false;
  return theCommands.EvaluateExpression(theCommands, input, output, notUsed, - 1);
}

bool Calculator::TimedOut() {
  if (theGlobalVariables.millisecondsMaxComputation <= 0) {
    return false;
  }
  int64_t elapsedMilliseconds = theGlobalVariables.GetElapsedMilliseconds();
  int64_t halfTimeLimit = theGlobalVariables.millisecondsMaxComputation / 2;
  if (elapsedMilliseconds <= halfTimeLimit) {
    return false;
  }
  if (!this->flagTimeLimitErrorDetected) {
    *this << "<br><b>Max allowed computational time is " << halfTimeLimit << " ms; "
    << "so far, " << elapsedMilliseconds
    << " have elapsed -> aborting computation ungracefully.</b>";
  }
  this->flagTimeLimitErrorDetected = true;
  this->flagAbortComputationASAP = true;
  return true;
}

bool Calculator::EvaluateExpression(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  bool& outputIsNonCacheable,
  int opIndexParentIfAvailable
) {
  RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::EvaluateExpression");
  theCommands.NumCallsEvaluateExpression ++;
  if (theCommands.flagLogFullTreeCrunching && theCommands.RecursionDeptH < 3) {
    theCommands << "<br>";
    for (int i = 0; i < theCommands.RecursionDeptH; i ++) {
      theCommands << "&nbsp&nbsp&nbsp&nbsp";
    }
    theCommands << "Evaluating " << input.Lispify()
    << " with rule stack cache index "
    << theCommands.RuleStackCacheIndex; // << this->RuleStack.ToString();
  }
  if (theCommands.RecursionDepthExceededHandleRoughly()) {
    return theCommands << " Evaluating expression: " << input.ToString() << " aborted. ";
  }
  output = input;
  if (output.IsError()) {
    theCommands.flagAbortComputationASAP = true;
    return true;
  }
  if (theCommands.EvaluatedExpressionsStack.Contains(input)) {
    std::stringstream errorStream;
    errorStream << "I think I have detected an infinite cycle: I am asked to reduce "
    << input.ToString()
    << " but I have already seen that expression in the expression stack. ";
    theCommands.flagAbortComputationASAP = true;
    return output.MakeError(errorStream.str(), theCommands);
  }
  //bool logEvaluationStepsRequested = theCommands.logEvaluationSteps.size >0;
  theCommands.EvaluatedExpressionsStack.AddOnTop(input);
  Expression theExpressionWithContext;
  theExpressionWithContext.reset(theCommands, 3);
  theExpressionWithContext.AddChildAtomOnTop(theCommands.opSequence());
  theExpressionWithContext.AddChildValueOnTop(theCommands.RuleStackCacheIndex);
  theExpressionWithContext.AddChildOnTop(input);
  int indexInCache = theCommands.cachedExpressions.GetIndex(theExpressionWithContext);
  if (indexInCache != - 1) {
    if (theCommands.flagLogCache) {
      theCommands << "<hr>Cache hit with state identifier "
      << theCommands.RuleStackCacheIndex << ": "
      << output.ToString() << " -> "
      << theCommands.imagesCachedExpressions[indexInCache].ToString();
      //theCommands << "DEBUG: "
      //<< theCommands.RuleStack.ToString();
    }
    output = theCommands.imagesCachedExpressions[indexInCache];
  } else {
    bool shouldCache =
    theCommands.cachedExpressions.size<theCommands.MaxCachedExpressionPerRuleStack &&
    !output.IsBuiltInTypE() && !output.IsBuiltInAtom();
    if (shouldCache) {
      theCommands.cachedExpressions.AddOnTop(theExpressionWithContext);
      indexInCache = theCommands.cachedExpressions.size - 1;
      theCommands.imagesCachedExpressions.SetSize(indexInCache + 1);
      theCommands.imagesCachedExpressions.LastObject()->reset(theCommands);
      theCommands.imagesCachedExpressions.LastObject()->MakeError("Error: not computed yet.", theCommands);
    }
  }
  //reduction phase:
  bool ReductionOcurred = true;
  int counterNumTransformations = 0;
  Expression transformationE(theCommands);
  Expression beforePatternMatch;
  std::string inputIfAtom;
  outputIsNonCacheable = false;
  //////////////////////////////////
  //std::stringstream logStream;
  //if (logEvaluationStepsRequested)
  //  logStream << "\\(" << output.ToString() << "\\)";
  bool doReportEvalRules =
  theGlobalVariables.flagReportEverything &&
  !theGlobalVariables.flagRunningCommandLine &&
  !theGlobalVariables.flagRunningApache;

  MemorySaving<Expression> oldChild;
  bool doExpressionHistory = (theCommands.historyStack.size > 0);
  if (doExpressionHistory) {
    theCommands.ExpressionHistoryAddEmptyHistory();
  }
  while (ReductionOcurred && !theCommands.flagAbortComputationASAP) {
    StateMaintainerCalculator theRuleStackMaintainer(theCommands);
    ReductionOcurred = false;
    counterNumTransformations ++;
    if (doExpressionHistory) {
      theCommands.ExpressionHistoryAdd(output, - 1);
    }
    if (output.IsAtom(&inputIfAtom)) {
      if (theCommands.atomsThatMustNotBeCached.Contains(inputIfAtom)) {
        outputIsNonCacheable = true;
      }
    }
    //The following code enclosed in the if clause is too
    //complicated/obfuscated for my taste
    //and perhaps needs a rewrite. However, at the moment of writing, I see
    //no better way of doing this, so here we go.
    if (outputIsNonCacheable) {
      if (indexInCache != - 1) {
        //We "undo" the caching process by
        //replacing the cached value with the minusOneExpression,
        //which, having no context, will never match another expression.
        theCommands.cachedExpressions.SetObjectAtIndex(indexInCache, theCommands.EMOne());
      }
      indexInCache = - 1;
    }
    if (indexInCache != - 1) {
      theCommands.imagesCachedExpressions[indexInCache] = output;
    }
    //////------Handling naughty expressions------
    if (theCommands.TimedOut()) {
      break;
    }
    if (counterNumTransformations > theCommands.MaxAlgTransformationsPerExpression) {
      if (!theCommands.flagMaxTransformationsErrorEncountered) {
        std::stringstream out;
        out << " While simplifying " << output.ToString(&theCommands.formatVisibleStrings)
        << "<br>Maximum number of algebraic transformations of "
        << theCommands.MaxAlgTransformationsPerExpression << " exceeded.";
        output.MakeError(out.str(), theCommands);
        theCommands.flagAbortComputationASAP = true;
        theCommands.flagMaxTransformationsErrorEncountered = true;
      }
      break;
    }
    //////------End of handling naughty expressions------
    //bool foundError = false;
    /////-------Children evaluation-------
    ProgressReport theReport;
    bool HistoryShouldBeRecorded = false;
    int historyStackSizeAtStart = - 1;
    if (doExpressionHistory) {
      historyStackSizeAtStart = theRuleStackMaintainer.GetCurrentHistory().size();
    }
    if (!output.IsFrozen()) {
      int indexOp = - 1;
      if (output.size() > 0) {
        if (output[0].IsAtom()) {
          indexOp = output[0].theData;
        }
      }
      for (int i = 0; i < output.size() && !theCommands.flagAbortComputationASAP; i ++) {
        if (i > 0 && output.StartsWith(theCommands.opEndStatement())) {
          if (doReportEvalRules) {
            std::stringstream reportStream;
            reportStream << "Substitution rules so far:";
            for (int j = 1; j < i; j ++) {
              if (
                output[j].StartsWith(theCommands.opDefine()) ||
                output[j].StartsWith(theCommands.opDefineConditional())
              ) {
                reportStream << "<br>" << MathRoutines::StringShortenInsertDots(
                  output[j].ToString(), 100
                );
              }
            }
            reportStream << "<br>Evaluating:<br><b>"
            << MathRoutines::StringShortenInsertDots(output[i].ToString(), 100) << "</b>";
            theReport.Report(reportStream.str());
          }
        }
        bool childIsNonCacheable = false;
        if (doExpressionHistory) {
          oldChild.GetElement() = output[i];
        }
        if (theCommands.EvaluateExpression(theCommands, output[i], transformationE, childIsNonCacheable, indexOp)) {
          output.SetChilD(i, transformationE);
        }
        if (doExpressionHistory) {
          if (transformationE != oldChild.GetElement()) {
            HistoryShouldBeRecorded = true;
          }
          Expression lastHistory = theRuleStackMaintainer.GetCurrentHistory();
          theCommands.ExpressionHistoryPop();
          theCommands.ExpressionHistoryAdd(lastHistory, i);
        }
        //important: here we check if the child is non-cache-able.
        if (childIsNonCacheable) {
          outputIsNonCacheable = true;
        }
        if (output[i].IsError()) {
          theCommands.flagAbortComputationASAP = true;
          break;
        }
        if (output.StartsWith(theCommands.opEndStatement())) {
          if (!theCommands.AccountRule(output[i], theRuleStackMaintainer)) {
            std::stringstream out;
            out << "Failed to account rule: " << output[i].ToString()
            << ". Most likely the cause is too deeply nested recursion. ";
            output.MakeError(out.str(), theCommands);
            theCommands.flagAbortComputationASAP = true;
            break;
          }
        }
      }
    }
    if (doExpressionHistory) {
      if (HistoryShouldBeRecorded) {
        theRuleStackMaintainer.GetCurrentHistoryRuleNames() = "Sub-expression simplification";
        theCommands.ExpressionHistoryAdd(output, - 1);
      } else {
        if (historyStackSizeAtStart > theRuleStackMaintainer.GetCurrentHistory().size()) {
          crash << "Error: we have historyStackSizeAtStart = " << historyStackSizeAtStart
          << " yet the expression history has size: "
          << theRuleStackMaintainer.GetCurrentHistory().size()
          << "<br>Expression history so far: "
          << theRuleStackMaintainer.GetCurrentHistory().ToString()
          << crash;
        }
        theRuleStackMaintainer.GetCurrentHistory().children.SetSize(historyStackSizeAtStart);
      }
    }
    /////-------Children evaluation end-------
    if (theCommands.flagAbortComputationASAP) {
      break;
    }
    /////-------Built-in evaluation-------
    if (theCommands.outerStandardFunction(theCommands, output, transformationE, opIndexParentIfAvailable)) {
      ReductionOcurred = true;
      if (theCommands.flagLogEvaluatioN) {
        theCommands << "<br>Rule context identifier: " << theCommands.RuleStackCacheIndex
        << "<br>" << HtmlRoutines::GetMathMouseHover(output.ToString()) << " -> "
        << HtmlRoutines::GetMathMouseHover(transformationE.ToString());
      }
      output = transformationE;
      continue;
    }
    /////-------Built-in evaluation end-------
    /////-------User-defined pattern matching------
    for (int i = 0; i < theCommands.RuleStack.size() && !theCommands.flagAbortComputationASAP; i ++) {
      const Expression& currentPattern = theCommands.RuleStack[i];
      theCommands.TotalNumPatternMatchedPerformed ++;
      if (theCommands.flagLogEvaluatioN) {
        beforePatternMatch = output;
      }
      MapList<Expression, Expression> bufferPairs;
      std::stringstream* theLog = theCommands.flagLogPatternMatching ? &theCommands.Comments : 0;
      if (theCommands.ProcessOneExpressionOnePatternOneSub(currentPattern, output, bufferPairs, theLog)) {
        ReductionOcurred = true;
        if (theCommands.flagLogEvaluatioN) {
          theCommands << "<hr>Rule cache index: " << theCommands.RuleStackCacheIndex
          << "<br>Rule: " << currentPattern.ToString()
          << "<br>" << HtmlRoutines::GetMathSpanPure(beforePatternMatch.ToString()) << " -> "
          << HtmlRoutines::GetMathSpanPure(output.ToString());
        }
        break;
      }
    }
/////-------User-defined pattern matching end------
  }
  theCommands.EvaluatedExpressionsStack.RemoveLastObject();
  if (theCommands.flagLogFullTreeCrunching && theCommands.RecursionDeptH < 3) {
    theCommands << "<br>";
    for (int i = 0; i < theCommands.RecursionDeptH; i ++) {
      theCommands << "&nbsp&nbsp&nbsp&nbsp";
    }
    theCommands << "to get: " << output.Lispify();
  }
  return true;
}

Expression* Calculator::PatternMatch(
  const Expression& thePattern,
  Expression& theExpression,
  MapList<Expression, Expression>& bufferPairs,
  const Expression* condition,
  std::stringstream* theLog
) {
  MacroRegisterFunctionWithName("Calculator::PatternMatch");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH >= this->MaxRecursionDeptH) {
    std::stringstream out;
    out << "Error: while trying to evaluate expression, the maximum recursion depth of "
    << this->MaxRecursionDeptH << " was exceeded";
    theExpression.MakeError(out.str(), *this);
    return 0;
  }
  thePattern.CheckInitialization();
  theExpression.CheckInitialization();
  if (!this->ExpressionMatchesPattern(thePattern, theExpression, bufferPairs, theLog)) {
    return 0;
  }
  if (theLog != 0) {
    (*theLog) << "<hr>found pattern: " << theExpression.ToString() << " -> "
    << thePattern.ToString() << " with " << bufferPairs.ToStringHtml();
  }
  if (condition == 0) {
    return &theExpression;
  }
  Expression tempExp = *condition;
  tempExp.CheckInitialization();
  if (theLog != 0) {
    (*theLog) << "<hr>Specializing condition pattern: " << tempExp.ToString();
  }
  this->SpecializeBoundVars(tempExp, bufferPairs);
  tempExp.CheckInitialization();
  if (theLog != 0) {
    (*theLog) << "<hr>Specialized condition: " << tempExp.ToString() << "; evaluating...";
  }
  Expression conditionResult;
  this->EvaluateExpression(*this, tempExp, conditionResult);
  if (theLog != 0) {
    (*theLog) << "<hr>The evaluated specialized condition: " << conditionResult.ToString()
    << "; evaluating...";
  }
  if (conditionResult.IsEqualToOne()) {
    return &theExpression;
  }
  return 0;
}

void Calculator::SpecializeBoundVars(Expression& toBeSubbedIn, MapList<Expression, Expression>& matchedPairs) {
  MacroRegisterFunctionWithName("Calculator::SpecializeBoundVars");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (toBeSubbedIn.IsListOfTwoAtomsStartingWith(this->opBind())) {
    if (matchedPairs.Contains(toBeSubbedIn)) {
      toBeSubbedIn = matchedPairs.GetValueCreate(toBeSubbedIn);
      //this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+ 1, MaxRecursionDepth);
      return;
    }
  }
  Expression subbedE;
  for (int i = 0; i < toBeSubbedIn.size(); i ++) {
    subbedE = toBeSubbedIn[i];
    this->SpecializeBoundVars(subbedE, matchedPairs);
    toBeSubbedIn.SetChilD(i, subbedE);
  }
//  this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+ 1, MaxRecursionDepth);
}

bool Calculator::ProcessOneExpressionOnePatternOneSub(
  const Expression& thePattern,
  Expression& theExpression,
  MapList<Expression, Expression>& bufferPairs,
  std::stringstream* theLog
) {
  MacroRegisterFunctionWithName("Calculator::ProcessOneExpressionOnePatternOneSub");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (!thePattern.StartsWith(this->opDefine(), 3) && !thePattern.StartsWith(this->opDefineConditional(), 4)) {
    return false;
  }
  if (theLog != 0) {
    (*theLog) << "<hr>attempting to reduce expression " << theExpression.ToString();
    (*theLog) << " by pattern " << thePattern.ToString();
  }
  theExpression.CheckInitialization();
  const Expression& currentPattern = thePattern[1];
  const Expression* theCondition = 0;
  bool isConditionalDefine = thePattern.StartsWith(this->opDefineConditional(), 4);
  if (isConditionalDefine) {
    theCondition = &thePattern[2];
  }
  Expression* toBeSubbed = this->PatternMatch(
    currentPattern, theExpression, bufferPairs, theCondition, theLog
  );
  if (toBeSubbed == 0) {
    return false;
  }
  if (theLog != 0) {
    *theLog << "<br><b>found a match!</b>";
  }
  if (isConditionalDefine) {
    *toBeSubbed = thePattern[3];
  } else {
    *toBeSubbed = thePattern[2];
  }
  this->SpecializeBoundVars(*toBeSubbed, bufferPairs);
  return true;
}

bool Calculator::ParseAndExtractExpressions(
  const std::string& theInputString,
  Expression& outputExp,
  List<SyntacticElement>& outputSynSoup,
  List<SyntacticElement>& outputSynStack,
  std::string* outputSynErrors
) {
  MacroRegisterFunctionWithName("Calculator::ParseAndExtractExpressions");
  this->CurrentSyntacticStacK = &outputSynStack;
  this->CurrrentSyntacticSouP = &outputSynSoup;
  this->ParseFillDictionary(theInputString);
  bool result = this->ExtractExpressions(outputExp, outputSynErrors);
  this->CurrentSyntacticStacK = &this->syntacticStacK;
  this->CurrrentSyntacticSouP = &this->syntacticSouP;
  return result;
}

void Calculator::initComputationStats() {
  this->startTimeEvaluationMilliseconds = theGlobalVariables.GetElapsedMilliseconds();
  this->NumListsStart                   = ParallelComputing::NumListsCreated;
  this->NumListResizesStart             = ParallelComputing::NumListResizesTotal;
  this->NumHashResizesStart             = ParallelComputing::NumHashResizes;
  this->NumSmallAdditionsStart          = Rational::TotalSmallAdditions;
  this->NumSmallMultiplicationsStart    = Rational::TotalSmallMultiplications;
  this->NumSmallGCDcallsStart           = Rational::TotalSmallGCDcalls;
  this->NumLargeAdditionsStart          = Rational::TotalLargeAdditions;
  this->NumLargeMultiplicationsStart    = Rational::TotalLargeMultiplications;
  this->NumLargeGCDcallsStart           = Rational::TotalLargeGCDcalls;
}

void Calculator::Evaluate(const std::string& theInput) {
  MacroRegisterFunctionWithName("Calculator::Evaluate");
  this->initComputationStats();
  this->inputString = theInput;
  this->ParseAndExtractExpressions(
    theInput, this->theProgramExpression, this->syntacticSouP, this->syntacticStacK, &this->syntaxErrors
  );
  this->EvaluateCommands();
}

void Calculator::EvaluateCommands() {
  MacroRegisterFunctionWithName("Calculator::EvaluateCommands");
  std::stringstream out;
  if (this->syntaxErrors != "") {
    if (!theGlobalVariables.flagRunningCommandLine) {
      out << "<hr><b>Syntax errors encountered</b><br>";
    } else {
      out << logger::consoleRed() << "Syntax errors encountered: " << logger::consoleNormal();
    }
    this->outputJS[WebAPI::result::syntaxErrors] = this->syntaxErrors;
    out << this->syntaxErrors;
    out << "<hr>";
  }
  Expression StartingExpression = this->theProgramExpression;
  this->flagAbortComputationASAP = false;
  this->Comments.clear();
  ProgressReport theReport;
  if (!theGlobalVariables.flagRunningCommandLine) {
    theReport.Report("Evaluating expressions, current expression stack:\n");
  }
  this->EvaluateExpression(*this, this->theProgramExpression, this->theProgramExpression);
  if (this->RecursionDeptH != 0) {
    crash << "This is a programming error: the starting recursion "
    << "depth before evaluation was 0, but after evaluation it is "
    << this->RecursionDeptH << "." << crash;
  }
  theGlobalVariables.theDefaultFormat.GetElement().flagMakingExpressionTableWithLatex = true;
  theGlobalVariables.theDefaultFormat.GetElement().flagUseLatex = true;
  theGlobalVariables.theDefaultFormat.GetElement().flagExpressionNewLineAllowed = true;
  theGlobalVariables.theDefaultFormat.GetElement().flagIncludeExtraHtmlDescriptionsInPlots = !this->flagPlotNoControls;
  theGlobalVariables.theDefaultFormat.GetElement().flagLatexDetailsInHtml = this->flagWriteLatexPlots;
  theGlobalVariables.theDefaultFormat.GetElement().flagExpressionIsFinal = true;
  if (theGlobalVariables.flagRunningCommandLine) {
    theGlobalVariables.theDefaultFormat.GetElement().flagUseQuotes = false;
    theGlobalVariables.theDefaultFormat.GetElement().flagExpressionIsFinal = true;
    if (theGlobalVariables.programArguments.size > 1) {
      out << "Input: " << logger::consoleYellow()
      << StartingExpression.ToString(&theGlobalVariables.theDefaultFormat.GetElement()) << std::endl;
    }
    theGlobalVariables.theDefaultFormat.GetElement().flagExpressionIsFinal = true;
    this->theObjectContainer.resetSliders();
    out << logger::consoleNormal() << "Output: " << logger::consoleGreen()
    << this->theProgramExpression.ToString(&theGlobalVariables.theDefaultFormat.GetElement())
    << logger::consoleNormal() << std::endl;
  } else if (!this->flagDisplayFullExpressionTree) {
    std::string badCharsString = this->ToStringIsCorrectAsciiCalculatorString(this->inputString);
    if (badCharsString != "") {
      out << badCharsString << "<hr>";
      this->outputJS[WebAPI::result::badInput] = badCharsString;
    }
    this->theObjectContainer.resetSliders();
    this->theObjectContainer.resetPlots();
    std::string javascriptString = this->theObjectContainer.ToStringJavascriptForUserInputBoxes();
    if (javascriptString != "") {
      this->outputJS["javascriptForUserInputBoxes"] = javascriptString;
    }
    out << javascriptString;
    JSData result;
    result.theType = JSData::token::tokenObject;
    std::string resultString = this->theProgramExpression.ToString(
      &theGlobalVariables.theDefaultFormat.GetElement(), &StartingExpression, true, &result
    );
    this->outputJS[WebAPI::result::resultLabel] = result;
    out << resultString;
  } else {
    std::string badCharsString = this->ToStringIsCorrectAsciiCalculatorString(this->inputString);
    if (badCharsString != "") {
      out << badCharsString << "<hr>";
      this->outputJS[WebAPI::result::badInput] = badCharsString;
    }
    this->theObjectContainer.resetSliders();
    out << "<hr>Input:<br> " << StartingExpression.ToStringFull() << "<hr>"
    << "Output:<br>" << this->theProgramExpression.ToStringFull();
    this->outputJS[WebAPI::result::resultLabel]["input"] = StartingExpression.ToStringFull();
    this->outputJS[WebAPI::result::resultLabel]["output"] = this->theProgramExpression.ToStringFull();
  }
  this->outputString = out.str();
  this->outputJS[WebAPI::result::resultHtml] = out.str();
  std::stringstream commentsStream;
  if (this->theObjectContainer.theAlgebraicClosure.theBasisMultiplicative.size > 1) {
    commentsStream << "<b>Algebraic closure status.</b><br>"
    << this->theObjectContainer.theAlgebraicClosure.ToString();
  }
  if (this->Comments.str() != "") {
    commentsStream << "<br><span>" << this->Comments.str() << "</span>";
  }
  this->outputCommentsString = commentsStream.str();
  this->outputJS[WebAPI::result::comments] = this->outputCommentsString;
  if (theGlobalVariables.flagRunningCommandLine && this->Comments.str() != "") {
    this->outputString += this->outputCommentsString;
  }
}