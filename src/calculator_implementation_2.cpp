// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "string_constants.h"

JSData Calculator::OperationHandlers::toJSON() {
  JSData result;
  JSData currentFunctionListDirect;
  currentFunctionListDirect.theType = JSData::token::tokenArray;
  for (int i = 0; i < this->handlers.size; i ++) {
    Function& currentHandler = this->handlers[i];
    currentFunctionListDirect.theList.addOnTop(currentHandler.toJSON());
  }
  JSData currentFunctionListComposite;
  currentFunctionListComposite.theType = JSData::token::tokenArray;
  for (int i = 0; i < this->compositeHandlers.size; i ++) {
    Function& currentHandler = this->compositeHandlers[i];
    currentFunctionListComposite.theList.addOnTop(currentHandler.toJSON());
  }
  result["regular"] = currentFunctionListDirect;
  result["composite"] = currentFunctionListComposite;
  return result;
}

JSData Calculator::ToJSONFunctionHandlers() {
  MacroRegisterFunctionWithName("Calculator::ToJSONFunctionHandlers");
  JSData output;
  output.theType = JSData::token::tokenObject;
  for (int i = 0; i < this->operations.size(); i ++) {
    if (this->operations.theValues[i].IsZeroPointer()) {
      continue;
    }
    const std::string& operationName = this->operations.theKeys[i];
    Calculator::OperationHandlers& handlers = this->operations.theValues[i].getElement();
    output[operationName] = handlers.toJSON();
  }
  return output;
}

Calculator::OperationHandlers::OperationHandlers() {
  this->flagDeallocated = false;
}

Calculator::OperationHandlers::~OperationHandlers() {
  this->flagDeallocated = true;
}

bool Calculator::OperationHandlers::CheckConsisitency() {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of Calculator::AtomHandler. " << global.fatal;
  }
  return true;
}

Calculator::NamedRuleLocation::NamedRuleLocation() {
  this->containerOperation = "";
  this->index = - 1;
  this->isComposite = false;
}

Expression Calculator::EZero() {
  Expression result;
  result.AssignValue(0, *this);
  return result;
}

Expression Calculator::EOne() {
  Expression result;
  result.AssignValue(1, *this);
  return result;
}

Expression Calculator::EMOne() {
  Expression result;
  result.AssignValue(- 1, *this);
  return result;
}

Expression Calculator::EFour() {
  Expression result;
  result.AssignValue(4, *this);
  return result;
}

Expression Calculator::ETwo() {
  Expression result;
  result.AssignValue(2, *this);
  return result;
}

Expression Calculator::EMHalf() {
  Expression result;
  result.AssignValue(Rational(- 1, 2), *this);
  return result;
}

Expression Calculator::EInfinity() {
  Expression result;
  result.makeAtom(this->opInfinity(), *this);
  return result;
}

Expression Calculator::EMInfinity() {
  Expression result;
  result = this->EMOne() * this->EInfinity();
  return result;
}

List<Function> Calculator::OperationHandlers::mergeHandlers() {
  List<Function> result;
  result.addListOnTop(this->handlers);
  result.addListOnTop(this->compositeHandlers);
  return result;
}

std::string Calculator::OperationHandlers::ToStringRuleStatusUser() {
  std::stringstream out;
  for (int i = 0; i < this->handlers.size; i ++) {
    Function& currentHandler = this->handlers[i];
    if (
      currentHandler.options.disabledByUser ==
      currentHandler.options.disabledByUserDefault
    ) {
      continue;
    }
    if (currentHandler.options.disabledByUser) {
      out << currentHandler.calculatorIdentifier << " is <b>off</b>. ";
    } else {
      out << currentHandler.calculatorIdentifier << " is <b>on</b>. ";
    }
  }
  return out.str();
}

std::string Calculator::ToStringRuleStatusUser() {
  MacroRegisterFunctionWithName("Calculator::ToStringRuleStatusUser");
  std::stringstream out;
  for (int i = 0; i < this->operations.size(); i ++) {
    if (this->operations.theValues[i].IsZeroPointer()) {
      continue;
    }
    out << this->operations.theValues[i].getElement().ToStringRuleStatusUser();
  }
  return out.str();
}

void Calculator::DoLogEvaluationIfNeedBe(Function& inputF) {
  if (!this->flagLogEvaluatioN) {
    return;
  }
  *this << "<hr>Built-in substitution: " << inputF.ToStringSummary()
  << "<br>" << global.GetElapsedSeconds() - this->LastLogEvaluationTime
  << " second(s) since last log entry. "
  << "Rule stack id: "
  << this->RuleStackCacheIndex << ", stack size: " << this->RuleStack.size();
  this->LastLogEvaluationTime = global.GetElapsedSeconds();
}

const List<Function>* Calculator::getOperationCompositeHandlers(int theOp) {
  if (theOp < 0 || theOp >= this->operations.size()) {
    // Instead of crashing, we may instead return nullptr.
    // TODO(tmilev): document why we are so harsh
    // in the crash message.
    // [note: I no longer remember the orginal rationale, if any].
    global.fatal << "Corrupt atom index: " << theOp << global.fatal;
  }
  if (this->operations.theValues[theOp].IsZeroPointer()) {
    return nullptr;
  }
  return &this->operations.theValues[theOp].GetElementConst().compositeHandlers;
}

const List<Function>* Calculator::getOperationHandlers(int theOp) {
  if (theOp < 0 || theOp >= this->operations.size()) {
    // Instead of crashing, we may instead return nullptr.
    // TODO(tmilev): document why we are so harsh
    // in the crash message.
    // [note: I no longer remember the original rationale, if any].
    global.fatal << "Corrupt atom index: " << theOp << global.fatal;
  }
  if (this->operations.theValues[theOp].IsZeroPointer()) {
    return nullptr;
  }
  return &this->operations.theValues[theOp].GetElementConst().handlers;
}

bool Calculator::outerStandardCompositeHandler(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  int opIndexParentIfAvailable,
  Function** outputHandler
) {
  if (!input.isList()) {
    return false;
  }
  const Expression& functionNameNode = input[0];
  if (!functionNameNode.startsWith()) {
    return false;
  }
  const List<Function>* theHandlers = theCommands.getOperationCompositeHandlers(functionNameNode[0].theData);
  if (theHandlers == nullptr) {
    return false;
  }
  for (int i = 0; i < theHandlers->size; i ++) {
    Function& currentHandler = (*theHandlers)[i];
    if (currentHandler.ShouldBeApplied(opIndexParentIfAvailable)) {
      if (currentHandler.Apply(
        theCommands, input, output, opIndexParentIfAvailable, outputHandler
      )) {
        theCommands.DoLogEvaluationIfNeedBe(currentHandler);
        return true;
      }
    }
  }
  return false;
}

bool Function::checkConsistency() const {
  if (this->options.visible != 0 && this->options.visible != 1) {
    global.fatal << "Invalid visible flag: " << this->options.visible << global.fatal;
  }
  return true;
}

bool Function::Apply(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  int opIndexParentIfAvailable,
  Function** outputHandler
) {
  if (!this->ShouldBeApplied(opIndexParentIfAvailable)) {
    return false;
  }
  if (this->theFunction == nullptr) {
    global.fatal << "Attempt to apply non-initialized function. " << global.fatal;
  }
  if (!this->options.flagIsInner) {
    if (this->theFunction(theCommands, input, output)) {
      if (output != input) {
        output.checkConsistency();
        theCommands.DoLogEvaluationIfNeedBe(*this);
        if (outputHandler != nullptr) {
          *outputHandler = this;
        }
        return true;
      }
    }
    return false;
  }
  if (this->inputFitsMyInnerType(input)) {
    if (this->theFunction(theCommands, input, output)) {
      output.checkConsistency();
      theCommands.DoLogEvaluationIfNeedBe(*this);
      if (outputHandler != nullptr) {
        *outputHandler = this;
      }
      return true;
    }
  }
  return false;
}

bool Calculator::outerStandardHandler(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  int opIndexParentIfAvailable,
  Function** outputHandler
) {
  const Expression& functionNameNode = input[0];
  int operationIndex = - 1;
  if (!functionNameNode.IsOperation(operationIndex)) {
    return false;
  }
  if (theCommands.operations.theValues[operationIndex].IsZeroPointer()) {
    return false;
  }
  const List<Function>& handlers = theCommands.operations.theValues[operationIndex].getElement().handlers;
  for (int i = 0; i < handlers.size; i ++) {
    Function& currentFunction = handlers[i];
    if (currentFunction.Apply(
      theCommands, input, output, opIndexParentIfAvailable, outputHandler
    )) {
      return true;
    }
  }
  return false;
}

bool Calculator::outerStandardFunction(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  int opIndexParentIfAvailable,
  Function** outputHandler
) {
  MacroRegisterFunctionWithName("Calculator::outerStandardFunction");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isList()) {
    return false;
  }
  if (theCommands.outerStandardCompositeHandler(
    theCommands, input, output, opIndexParentIfAvailable, outputHandler
  )) {
    return true;
  }
  if (theCommands.outerStandardHandler(
    theCommands, input, output, opIndexParentIfAvailable, outputHandler
  )) {
    return true;
  }
  return false;
}

bool Calculator::ExpressionMatchesPattern(
  const Expression& thePattern,
  const Expression& input,
  MapList<Expression, Expression>& matchedExpressions,
  std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("Calculator::ExpressionMatchesPattern");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (!(thePattern.owner == this && input.owner == this)) {
    global.fatal << "This is a programming error. "
    << "Either a pattern or an input has a wrongly  initialized owner: the pattern is "
    << thePattern.toString() << " and the input is "
    << input.toString() << ". The error is certainly in the preceding code; here "
    << "is a stack trace, however beware that the error might be in code preceding the stack loading. "
    << global.fatal;
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << " <hr> current input: " << input.toString() << "<br>current pattern: " << thePattern.toString();
    *commentsGeneral << "<br> current matched expressions: " << matchedExpressions.ToStringHtml();
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (this->RecursionDeptH>this->MaxRecursionDeptH) {
    std::stringstream out;
    out << "Max recursion depth of " << this->MaxRecursionDeptH << " exceeded whlie trying to match expression pattern "
    << thePattern.toString() << " onto expression " << input.toString();
    this->evaluationErrors.addOnTop(out.str());
    return false;
  }
//  if (this->opDefine() == input.theOperation)
//    return false;
  int opVarB = this->opBind();
  if (thePattern.IsListStartingWithAtom(opVarB)) {
    if (!matchedExpressions.contains(thePattern)) {
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
    if (!input.startsWith(thePattern.owner->opPlus())) {
      return false;
    }
    matchedExpressions.theValues.setSize(numMatchedExpressionsAtStart);
    matchedExpressions.theKeys.setSize(numMatchedExpressionsAtStart);
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
    global.fatal << "StackMaintainerCalculator has zero owner. " << global.fatal;
  }
  this->owner->RuleStack.addChildOnTop(theRule);
  std::string currentRule;
  if (
    theRule.startsWith(this->owner->opRulesOn()) ||
    theRule.startsWith(this->owner->opRulesOff())
  ) {
    for (int j = 1; j < theRule.size(); j ++) {
      if (!theRule[j].IsOfType(&currentRule)) {
        continue;
      }
      if (!this->owner->namedRules.contains(currentRule)) {
        continue;
      }
      this->owner->GetFunctionHandlerFromNamedRule(currentRule).options.disabledByUser =
      theRule.startsWith(this->owner->opRulesOff());
    }
  }
  this->owner->RuleStackCacheIndex = this->owner->cachedRuleStacks.getIndex(this->owner->RuleStack);
  if (this->owner->RuleStackCacheIndex == - 1) {
    if (this->owner->cachedRuleStacks.size < this->owner->MaxCachedExpressionPerRuleStack) {
      this->owner->RuleStackCacheIndex = this->owner->cachedRuleStacks.size;
      this->owner->cachedRuleStacks.addOnTop(this->owner->RuleStack);
    }
  }
  if (this->owner->flagLogRules) {
    *this->owner << "<hr>Added rule " << theRule.toString() << " with state identifier "
    << this->owner->RuleStackCacheIndex;
  }
}

StateMaintainerCalculator::StateMaintainerCalculator(Calculator& inputBoss) {
  this->owner = &inputBoss;
  this->startingRuleStackIndex = inputBoss.RuleStackCacheIndex;
  this->startingRuleStackSize = inputBoss.RuleStack.size();
}

StateMaintainerCalculator::~StateMaintainerCalculator() {
  if (this->owner == nullptr) {
    return;
  }
  Expression& theRuleStack = this->owner->RuleStack;
  std::string currentRuleName;
  bool shouldUpdateRules = false;
  for (int i = this->startingRuleStackSize; i < theRuleStack.size(); i ++) {
    if (
      theRuleStack[i].startsWith(this->owner->opRulesOn()) ||
      theRuleStack[i].startsWith(this->owner->opRulesOff())
    ) {
      for (int j = 1; j < theRuleStack[i].size(); j ++) {
        if (!theRuleStack[i][j].IsOfType<std::string>(&currentRuleName)) {
          continue;
        }
        if (!this->owner->namedRules.contains(currentRuleName)) {
          continue;
        }
        Function& currentRule = this->owner->GetFunctionHandlerFromNamedRule(currentRuleName);
        currentRule.options.disabledByUser = currentRule.options.disabledByUserDefault;
        shouldUpdateRules = true;
      }
    }
  }
  if (shouldUpdateRules) {
    for (int i = 0; i < this->startingRuleStackSize; i ++) {
      if (theRuleStack[i].startsWith(this->owner->opRulesOn())) {
        for (int j = 1; j < theRuleStack[i].size(); j ++) {
          Function& currentFun = this->owner->GetFunctionHandlerFromNamedRule(
            theRuleStack[i][j].getValue<std::string>()
          );
          currentFun.options.disabledByUser = false;
        }
      } else if (theRuleStack[i].startsWith(this->owner->opRulesOff())) {
        for (int j = 1; j < theRuleStack[i].size(); j ++) {
          Function& currentFun = this->owner->GetFunctionHandlerFromNamedRule(
            theRuleStack[i][j].getValue<std::string>()
          );
          currentFun.options.disabledByUser = true;
        }
      }
    }
  }
  this->owner->RuleStackCacheIndex = this->startingRuleStackIndex;
  this->owner->RuleStack.children.setSize(this->startingRuleStackSize);
  this->owner = nullptr;
}

Expression Calculator::GetNewBoundVar() {
  Expression result(*this);
  result.addChildAtomOnTop(this->opBind());
  result.addChildOnTop(this->GetNewAtom());
  return result;
}

Expression Calculator::GetNewAtom() {
  std::string atomPrefix;
  std::string candidate;
  while (true) {
    for (char a = 'a'; a <= 'z'; a ++) {
      candidate = atomPrefix;
      candidate.push_back(a);
      if (candidate == "d") {
        continue;
      }
      if (!this->operations.contains(candidate)) {
        Expression result;
        result.makeAtom(candidate, *this);
        return result;
      }
    }
    atomPrefix = candidate;
  }
}

bool Calculator::AccountRule(
  const Expression& ruleE, StateMaintainerCalculator& theRuleStackMaintainer
) {
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
  if (!ruleE[1].startsWith(this->opEndStatement())) {
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
  return theCommands.EvaluateExpression(theCommands, input, output, notUsed, - 1, nullptr);
}

bool Calculator::TimedOut() {
  if (global.millisecondsMaxComputation <= 0) {
    return false;
  }
  int64_t elapsedMilliseconds = global.GetElapsedMilliseconds();
  int64_t halfTimeLimit = global.millisecondsMaxComputation / 2;
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

Calculator::EvaluateLoop::EvaluateLoop(Calculator& inputOwner) {
  this->owner = &inputOwner;
  this->opIndexParent = - 1;
  this->flagIsNonCacheable = false;
  this->numberOfTransformations = 0;
  this->indexInCache = - 1;
  this->reductionOccurred = false;
  this->history = nullptr;
  this->outpuT = nullptr;
}

void Calculator::EvaluateLoop::AccountHistoryChildTransformation(
  const Expression& transformedChild,
  const Expression& childHistory,
  int childIndex
) {
  if (this->history == nullptr) {
    // History is not recorded.
    return;
  }
  if (this->currentChild == transformedChild) {
    // Child hasn't changed.
    return;
  }
  Expression incomingHistory, indexE;
  indexE.AssignValue(childIndex, *this->owner);
  incomingHistory.MakeXOX(
    *this->owner,
    this->owner->opExpressionHistorySetChild(),
    indexE,
    childHistory
  );
  this->history->addChildOnTop(incomingHistory);
}

void Calculator::EvaluateLoop::AccountHistory(Function* handler, const std::string& info) {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::AccountHistory");
  this->CheckInitialization();
  if (this->history == nullptr) {
    return;
  }
  Expression incomingHistory;
  if (this->history->size() > 0) {
    const Expression& lastHistory = (*this->history)[this->history->size() - 1];
    if (lastHistory.size() < 2) {
      global.fatal << "Unexpected history expression: "
      << lastHistory.toString() << global.fatal;
    }
    if (lastHistory[1] == *(this->outpuT)) {
      // Expression hasn't changed since last assignment.
      return;
    }
  }
  if (this->history->size() == 0) {
    this->history->reset(*(this->owner));
    this->history->addChildAtomOnTop(this->owner->opExpressionHistory());
  }
  std::stringstream description;
  if (handler != nullptr) {
    description << handler->calculatorIdentifier;
  } else {
    description << info;
  }
  Expression extraInformation;
  extraInformation.AssignValue(description.str(), *this->owner);
  incomingHistory.MakeXOX(
    *this->owner,
    this->owner->opExpressionHistorySet(),
    *(this->outpuT),
    extraInformation
  );
  incomingHistory.checkConsistency();
  this->history->addChildOnTop(incomingHistory);
}

bool Calculator::EvaluateLoop::SetOutput(
  const Expression& input, Function* handler, const std::string& info
) {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::SetOutput");
  if (this->outpuT == nullptr) {
    global.fatal << "Non-initialized evaluation loop. " << global.fatal;
  }
  *(this->outpuT) = input;
  this->AccountHistory(handler, info);
  return true;
}

void Calculator::EvaluateLoop::InitializeOneRun() {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::InitializeOneRun");
  this->numberOfTransformations ++;
  std::string atomValue;
  if (this->outpuT->IsOperation(&atomValue)) {
    if (this->owner->atomsThatMustNotBeCached.contains(atomValue)) {
      this->flagIsNonCacheable = true;
    }
  }
  // The following code is too
  // complicated/obfuscated for my taste
  // and perhaps needs a rewrite.
  // However, at the moment of writing, I see
  // no better way of doing this, so here we go.
  if (this->flagIsNonCacheable) {
    if (this->indexInCache != - 1) {
      // We "undo" the caching process by
      // replacing the cached value with the minusOneExpression,
      // which, having no context, will never match another expression.
      this->owner->cachedExpressions.setObjectAtIndex(
        this->indexInCache, this->owner->EMOne()
      );
    }
    this->indexInCache = - 1;
  }
  if (this->indexInCache != - 1) {
    this->owner->imagesCachedExpressions[indexInCache] = *(this->outpuT);
  }
}

bool Calculator::EvaluateLoop::OutputHasErrors() {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::OutputHasErrors");
  if (this->owner->TimedOut()) {
    return true;
  }
  if (
    this->numberOfTransformations <=
    this->owner->MaxAlgTransformationsPerExpression
  ) {
    return false;
  }
  if (!this->owner->flagMaxTransformationsErrorEncountered) {
    std::stringstream out;
    out << " While simplifying "
    << this->outpuT->toString(&this->owner->formatVisibleStrings)
    << "<br>Maximum number of algebraic transformations of "
    << this->owner->MaxAlgTransformationsPerExpression << " exceeded.";
    this->outpuT->MakeError(out.str(), *this->owner);
    this->reductionOccurred = true;
    this->owner->flagAbortComputationASAP = true;
    this->owner->flagMaxTransformationsErrorEncountered = true;
  }
  return true;
}

void Calculator::EvaluateLoop::ReportChildEvaluation(Expression& output, int childIndex) {
  if (!this->theReport.tickAndWantReport()) {
    return;
  }
  std::stringstream reportStream;
  reportStream << "substitution rules so far:";
  for (int j = 1; j < childIndex; j ++) {
    if (
      output[j].startsWith(this->owner->opDefine()) ||
      output[j].startsWith(this->owner->opDefineConditional())
    ) {
      reportStream << "<br>" << StringRoutines::StringShortenInsertDots(
        output[j].toString(), 100
      );
   }
  }
  reportStream << "<br>Evaluating:<br><b>"
  << StringRoutines::StringShortenInsertDots(output[childIndex].toString(), 100) << "</b>";
  theReport.report(reportStream.str());
}

bool Calculator::EvaluateLoop::EvaluateChildren(
  StateMaintainerCalculator& maintainRuleStack
) {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::EvaluateChildren");
  if (this->outpuT->IsFrozen()) {
    return true;
  }
  int indexOp = - 1;
  if (this->outpuT->size() > 0) {
    if ((*this->outpuT)[0].IsAtom()) {
      indexOp = (*this->outpuT)[0].theData;
    }
  }
  Expression childEvaluation, historyContainer;
  Expression* historyChild = nullptr;
  if (this->history != nullptr) {
    historyChild = &historyContainer;
  }
  for (int i = 0; i < this->outpuT->size(); i ++) {
    if (i > 0) {
      this->ReportChildEvaluation(*this->outpuT, i);
    }
    bool childIsNonCacheable = false;
    if (this->history != nullptr) {
      this->currentChild = (*this->outpuT)[i];
    }
    if (this->owner->EvaluateExpression(
      *this->owner,
      (*this->outpuT)[i],
      childEvaluation,
      childIsNonCacheable,
      indexOp,
      historyChild
    )) {
      this->outpuT->setChild(i, childEvaluation);
    }
    this->AccountHistoryChildTransformation(childEvaluation, historyContainer, i);
    // If the child is non-cache-able, so is the current one.
    // Once evaluation has passed through a non-cacheable expression,
    // our expression is no longer cache-able.
    if (childIsNonCacheable) {
      this->flagIsNonCacheable = true;
    }
    if ((*this->outpuT)[i].IsError()) {
      this->owner->flagAbortComputationASAP = true;
      return false;
    }
    if (this->outpuT->startsWith(this->owner->opEndStatement())) {
      if (!this->owner->AccountRule((*this->outpuT)[i], maintainRuleStack)) {
        std::stringstream out;
        out
        << "Failed to account rule: " << (*this->outpuT)[i].toString()
        << ". Most likely the cause is too deeply nested recursion. ";
        this->outpuT->MakeError(out.str(), *this->owner);
        this->owner->flagAbortComputationASAP = true;
      }
    }
    if (this->owner->flagAbortComputationASAP) {
      return false;
    }
  }
  return true;
}

bool Calculator::EvaluateLoop::UserDefinedEvaluation() {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::UserDefinedEvaluation");
  Expression beforePatternMatch, afterPatternMatch;
  for (
    int i = 0;
    i < this->owner->RuleStack.size() && !this->owner->flagAbortComputationASAP;
    i ++
  ) {
    const Expression& currentPattern = this->owner->RuleStack[i];
    this->owner->TotalNumPatternMatchedPerformed ++;
    if (this->owner->flagLogEvaluatioN) {
      beforePatternMatch = *this->outpuT;
    }
    MapList<Expression, Expression> bufferPairs;
    std::stringstream* theLog = this->owner->flagLogPatternMatching ? &this->owner->comments : nullptr;
    afterPatternMatch = *(this->outpuT);
    if (this->owner->ProcessOneExpressionOnePatternOneSub(
      currentPattern, afterPatternMatch, bufferPairs, theLog
    )) {
      std::stringstream substitutionComment;
      if (this->history == nullptr) {
        substitutionComment << "User-defined substition: " << currentPattern.toString();
      }
      this->SetOutput(afterPatternMatch, nullptr, substitutionComment.str());
      this->reductionOccurred = true;
      if (this->owner->flagLogEvaluatioN) {
        *this->owner
        << "<hr>Rule cache index: " << this->owner->RuleStackCacheIndex
        << "<br>Rule: " << currentPattern.toString() << "<br>"
        << HtmlRoutines::GetMathSpanPure(beforePatternMatch.toString())
        << " -> " << HtmlRoutines::GetMathSpanPure(this->outpuT->toString());
      }
      return true;
    }
  }
  return false;
}

bool Calculator::EvaluateLoop::CheckInitialization() {
  if (this->owner == nullptr || this->outpuT == nullptr) {
    global.fatal << "Non-initialized evaluation loop. " << global.fatal;
  }
  return true;
}

bool Calculator::EvaluateLoop::BuiltInEvaluation() {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::BuiltInEvaluation");
  this->CheckInitialization();
  Expression result;
  Function* handlerContainer = nullptr;
  if (!this->owner->outerStandardFunction(
    *(this->owner), *(this->outpuT), result, this->opIndexParent, &handlerContainer
  )) {
    return false;
  }
  this->reductionOccurred = true;
  if (this->owner->flagLogEvaluatioN) {
    *(this->owner) << "<br>Rule context identifier: "
    << this->owner->RuleStackCacheIndex
    << "<br>" << HtmlRoutines::GetMathMouseHover(this->outpuT->toString())
    << " -> " << HtmlRoutines::GetMathMouseHover(result.toString());
  }
  return this->SetOutput(result, handlerContainer, "");
}

bool Calculator::EvaluateLoop::ReduceOnce() {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::ReduceOnce");
  StateMaintainerCalculator maintainRuleStack(*(this->owner));
  this->CheckInitialization();
  this->InitializeOneRun();
  if (this->OutputHasErrors()) {
    return false;
  }
  if (!this->EvaluateChildren(maintainRuleStack)) {
    return false;
  }
  if (this->owner->flagAbortComputationASAP) {
    return false;
  }
  if (this->BuiltInEvaluation()) {
    return true;
  }
  if (this->UserDefinedEvaluation()) {
    return true;
  }
  return false;
}

void Calculator::EvaluateLoop::LookUpCache() {
  this->owner->EvaluatedExpressionsStack.addOnTop(*(this->outpuT));
  Expression theExpressionWithContext;
  theExpressionWithContext.reset(*this->owner, 3);
  theExpressionWithContext.addChildAtomOnTop(this->owner->opSequence());
  theExpressionWithContext.AddChildValueOnTop(this->owner->RuleStackCacheIndex);
  theExpressionWithContext.addChildOnTop(*(this->outpuT));
  this->indexInCache = this->owner->cachedExpressions.getIndex(theExpressionWithContext);
  if (this->indexInCache != - 1) {
    if (this->owner->flagLogCache) {
      *this->owner << "<hr>Cache hit with state identifier "
      << this->owner->RuleStackCacheIndex << ": "
      << this->outpuT->toString() << " -> "
      << this->owner->imagesCachedExpressions[this->indexInCache].toString();
    }
    std::stringstream comment;
    if (this->history != nullptr) {
      comment << "Previously computed that \\("
      << this->outpuT->toString() << " = "
      << this->owner->imagesCachedExpressions[this->indexInCache].toString()
      << "\\)";
    }
    this->SetOutput(this->owner->imagesCachedExpressions[this->indexInCache], nullptr, comment.str());
    return;
  }
  if (
    this->owner->cachedExpressions.size > this->owner->MaxCachedExpressionPerRuleStack ||
    this->outpuT->IsBuiltInTypE() ||
    this->outpuT->IsBuiltInAtom()
  ) {
    return;
  }
  this->owner->cachedExpressions.addOnTop(theExpressionWithContext);
  this->indexInCache = this->owner->cachedExpressions.size - 1;
  this->owner->imagesCachedExpressions.setSize(this->indexInCache + 1);
  this->owner->imagesCachedExpressions.lastObject()->MakeError("Error: not computed yet.", *this->owner);
}

bool Calculator::EvaluateExpression(
  Calculator& theCommands,
  const Expression& input,
  Expression& outpuT,
  bool& outputIsNonCacheable,
  int opIndexParentIfAvailable,
  Expression* outputHistory
) {
  RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::EvaluateExpression");
  theCommands.stats.expressionEvaluated ++;
  theCommands.stats.callsSinceReport ++;
  Calculator::EvaluateLoop state(theCommands);
  state.outpuT = &outpuT;
  state.history = outputHistory;
  if (state.history != nullptr) {
    state.history->reset(theCommands);
  }
  state.opIndexParent = opIndexParentIfAvailable;
  if (
    theCommands.stats.callsSinceReport >=
    theCommands.stats.maximumCallsBeforeReportGeneration
  ) {
    theCommands.stats.callsSinceReport = 0;
    std::stringstream reportStream;
    reportStream << "Evaluating: " << input.toString();
    state.theReport.report(reportStream.str());
  }
  if (theCommands.flagLogFullTreeCrunching && theCommands.RecursionDeptH < 3) {
    theCommands << "<br>";
    for (int i = 0; i < theCommands.RecursionDeptH; i ++) {
      theCommands << "&nbsp&nbsp&nbsp&nbsp";
    }
    theCommands << "Evaluating " << input.Lispify()
    << " with rule stack cache index "
    << theCommands.RuleStackCacheIndex; // << this->RuleStack.toString();
  }
  if (theCommands.RecursionDepthExceededHandleRoughly()) {
    return theCommands << " Evaluating expression: " << input.toString() << " aborted. ";
  }
  state.SetOutput(input, nullptr, "");
  if (state.outpuT->IsError()) {
    theCommands.flagAbortComputationASAP = true;
    return true;
  }
  if (theCommands.EvaluatedExpressionsStack.contains(input)) {
    std::stringstream errorStream;
    errorStream << "I think I have detected an infinite cycle: I am asked to reduce "
    << input.toString()
    << " but I have already seen that expression in the expression stack. ";
    theCommands.flagAbortComputationASAP = true;
    Expression errorE;
    errorE.MakeError(errorStream.str(), theCommands);
    return state.SetOutput(errorE, nullptr, "Error");
  }
  //bool logEvaluationStepsRequested = theCommands.logEvaluationSteps.size > 0;
  state.LookUpCache();
  // reduction phase:
  //////////////////////////////////
  // EvaluateExpression is called recursively
  // inside state.EvaluateChildren
  // inside state.ReduceOnce.
  while (state.ReduceOnce()) {
  }
  outputIsNonCacheable = state.flagIsNonCacheable;
  theCommands.EvaluatedExpressionsStack.removeLastObject();
  if (theCommands.flagLogFullTreeCrunching && theCommands.RecursionDeptH < 3) {
    theCommands << "<br>";
    for (int i = 0; i < theCommands.RecursionDeptH; i ++) {
      theCommands << "&nbsp&nbsp&nbsp&nbsp";
    }
    theCommands << "to get: " << state.outpuT->Lispify();
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
    out << "Error: while trying to evaluate expression, "
    << "the maximum recursion depth of "
    << this->MaxRecursionDeptH << " was exceeded";
    theExpression.MakeError(out.str(), *this);
    return nullptr;
  }
  thePattern.CheckInitialization();
  theExpression.CheckInitialization();
  if (!this->ExpressionMatchesPattern(
    thePattern, theExpression, bufferPairs, theLog
  )) {
    return nullptr;
  }
  if (theLog != nullptr) {
    (*theLog) << "<hr>found pattern: " << theExpression.toString() << " -> "
    << thePattern.toString() << " with " << bufferPairs.ToStringHtml();
  }
  if (condition == nullptr) {
    return &theExpression;
  }
  Expression tempExp = *condition;
  tempExp.CheckInitialization();
  if (theLog != nullptr) {
    (*theLog) << "<hr>Specializing condition pattern: " << tempExp.toString();
  }
  this->SpecializeBoundVars(tempExp, bufferPairs);
  tempExp.CheckInitialization();
  if (theLog != nullptr) {
    (*theLog) << "<hr>Specialized condition: " << tempExp.toString() << "; evaluating...";
  }
  Expression conditionResult;
  this->EvaluateExpression(*this, tempExp, conditionResult);
  if (theLog != nullptr) {
    (*theLog) << "<hr>The evaluated specialized condition: " << conditionResult.toString()
    << "; evaluating...";
  }
  if (conditionResult.IsEqualToOne()) {
    return &theExpression;
  }
  return nullptr;
}

void Calculator::SpecializeBoundVars(Expression& toBeSubbedIn, MapList<Expression, Expression>& matchedPairs) {
  MacroRegisterFunctionWithName("Calculator::SpecializeBoundVars");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (toBeSubbedIn.IsListOfTwoAtomsStartingWith(this->opBind())) {
    if (matchedPairs.contains(toBeSubbedIn)) {
      toBeSubbedIn = matchedPairs.GetValueCreate(toBeSubbedIn);
      //this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+ 1, MaxRecursionDepth);
      return;
    }
  }
  Expression subbedE;
  for (int i = 0; i < toBeSubbedIn.size(); i ++) {
    subbedE = toBeSubbedIn[i];
    this->SpecializeBoundVars(subbedE, matchedPairs);
    toBeSubbedIn.setChild(i, subbedE);
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
  if (
    !thePattern.startsWith(this->opDefine(), 3) &&
    !thePattern.startsWith(this->opDefineConditional(), 4)
  ) {
    return false;
  }
  if (theLog != nullptr) {
    (*theLog) << "<hr>attempting to reduce expression " << theExpression.toString();
    (*theLog) << " by pattern " << thePattern.toString();
  }
  theExpression.CheckInitialization();
  const Expression& currentPattern = thePattern[1];
  const Expression* theCondition = nullptr;
  bool isConditionalDefine = thePattern.startsWith(this->opDefineConditional(), 4);
  if (isConditionalDefine) {
    theCondition = &thePattern[2];
  }
  Expression* toBeSubbed = this->PatternMatch(
    currentPattern, theExpression, bufferPairs, theCondition, theLog
  );
  if (toBeSubbed == nullptr) {
    return false;
  }
  if (theLog != nullptr) {
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

bool Calculator::Parse(const std::string& input, Expression& output) {
  List<SyntacticElement> syntacticSoup, syntacticStack;
  return this->ParseAndExtractExpressions(input, output, syntacticSoup, syntacticStack, nullptr);
}

bool Calculator::ParseAndExtractExpressions(
  const std::string& input,
  Expression& output,
  List<SyntacticElement>& outputSynSoup,
  List<SyntacticElement>& outputSynStack,
  std::string* outputSynErrors
) {
  MacroRegisterFunctionWithName("Calculator::ParseAndExtractExpressions");
  this->CurrentSyntacticStacK = &outputSynStack;
  this->CurrrentSyntacticSouP = &outputSynSoup;
  this->ParseFillDictionary(input);
  bool result = this->ExtractExpressions(output, outputSynErrors);
  this->CurrentSyntacticStacK = &this->syntacticStacK;
  this->CurrrentSyntacticSouP = &this->syntacticSouP;
  return result;
}

void Calculator::initComputationStats() {
  this->startTimeEvaluationMilliseconds = global.GetElapsedMilliseconds();
  this->NumListsStart                   = static_cast<signed>( ParallelComputing::NumListsCreated    );
  this->NumListResizesStart             = static_cast<signed>( ParallelComputing::NumListResizesTotal);
  this->NumHashResizesStart             = static_cast<signed>( ParallelComputing::NumHashResizes     );
  this->NumSmallAdditionsStart          = static_cast<signed>( Rational::TotalSmallAdditions         );
  this->NumSmallMultiplicationsStart    = static_cast<signed>( Rational::TotalSmallMultiplications   );
  this->NumSmallGCDcallsStart           = static_cast<signed>( Rational::TotalSmallGCDcalls          );
  this->NumLargeAdditionsStart          = static_cast<signed>( Rational::TotalLargeAdditions         );
  this->NumLargeMultiplicationsStart    = static_cast<signed>( Rational::TotalLargeMultiplications   );
  this->NumLargeGCDcallsStart           = static_cast<signed>( Rational::TotalLargeGCDcalls          );
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
    if (!global.flagRunningConsoleRegular) {
      out << "<hr><b>Syntax errors encountered</b><br>";
    } else {
      out << Logger::consoleRed() << "Syntax errors encountered: " << Logger::consoleNormal();
    }
    this->outputJS[WebAPI::result::syntaxErrors] = this->syntaxErrors;
    out << this->syntaxErrors;
    out << "<hr>";
  }
  Expression StartingExpression = this->theProgramExpression;
  this->flagAbortComputationASAP = false;
  this->comments.clear();
  ProgressReport theReport;
  if (!global.flagRunningConsoleRegular) {
    theReport.report("Evaluating expressions, current expression stack:\n");
  }
  this->EvaluateExpression(*this, this->theProgramExpression, this->theProgramExpression);
  if (this->RecursionDeptH != 0) {
    global.fatal << "This is a programming error: the starting recursion "
    << "depth before evaluation was 0, but after evaluation it is "
    << this->RecursionDeptH << "." << global.fatal;
  }
  global.theDefaultFormat.getElement().flagMakingExpressionTableWithLatex = true;
  global.theDefaultFormat.getElement().flagUseLatex = true;
  global.theDefaultFormat.getElement().flagExpressionNewLineAllowed = true;
  global.theDefaultFormat.getElement().flagIncludeExtraHtmlDescriptionsInPlots = !this->flagPlotNoControls;
  global.theDefaultFormat.getElement().flagLatexDetailsInHtml = this->flagWriteLatexPlots;
  global.theDefaultFormat.getElement().flagExpressionIsFinal = true;
  if (global.flagRunningConsoleRegular) {
    global.theDefaultFormat.getElement().flagUseQuotes = false;
    global.theDefaultFormat.getElement().flagExpressionIsFinal = true;
    if (global.programArguments.size > 1) {
      out << "Input: " << Logger::consoleYellow()
      << StartingExpression.toString(&global.theDefaultFormat.getElement()) << std::endl;
    }
    global.theDefaultFormat.getElement().flagExpressionIsFinal = true;
    this->theObjectContainer.resetSliders();
    out << Logger::consoleNormal() << "Output: " << Logger::consoleGreen()
    << this->theProgramExpression.toString(&global.theDefaultFormat.getElement())
    << Logger::consoleNormal() << std::endl;
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
    std::string resultString = this->theProgramExpression.toString(
      &global.theDefaultFormat.getElement(), &StartingExpression, true, &result
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
    out << "<hr>Input:<br> " << StartingExpression.toStringFull() << "<hr>"
    << "Output:<br>" << this->theProgramExpression.toStringFull();
    this->outputJS[WebAPI::result::resultLabel]["input"] = StartingExpression.toStringFull();
    this->outputJS[WebAPI::result::resultLabel]["output"] = this->theProgramExpression.toStringFull();
  }
  this->outputString = out.str();
  this->outputJS[WebAPI::result::resultHtml] = out.str();
  std::stringstream commentsStream;
  if (this->theObjectContainer.theAlgebraicClosure.latestBasis.size > 1) {
    commentsStream << "<b>Algebraic closure status.</b><br>"
    << this->theObjectContainer.theAlgebraicClosure.toString();
  }
  if (this->comments.str() != "") {
    commentsStream << "<br><span>" << this->comments.str() << "</span>";
  }
  this->outputCommentsString = commentsStream.str();
  this->outputJS[WebAPI::result::comments] = this->outputCommentsString;
  if (global.flagRunningConsoleRegular && this->comments.str() != "") {
    this->outputString += this->outputCommentsString;
  }
}
