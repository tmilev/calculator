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

JSData Calculator::toJSONFunctionHandlers() {
  MacroRegisterFunctionWithName("Calculator::toJSONFunctionHandlers");
  JSData output;
  output.theType = JSData::token::tokenObject;
  for (int i = 0; i < this->operations.size(); i ++) {
    if (this->operations.theValues[i].isZeroPointer()) {
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

bool Calculator::OperationHandlers::checkConsistency() {
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

Expression Calculator::expressionZero() {
  Expression result;
  result.assignValue(0, *this);
  return result;
}

Expression Calculator::expressionOne() {
  Expression result;
  result.assignValue(1, *this);
  return result;
}

Expression Calculator::expressionMinusOne() {
  Expression result;
  result.assignValue(- 1, *this);
  return result;
}

Expression Calculator::expressionFour() {
  Expression result;
  result.assignValue(4, *this);
  return result;
}

Expression Calculator::expressionTwo() {
  Expression result;
  result.assignValue(2, *this);
  return result;
}

Expression Calculator::expressionMinusHalf() {
  Expression result;
  result.assignValue(Rational(- 1, 2), *this);
  return result;
}

Expression Calculator::expressionInfinity() {
  Expression result;
  result.makeAtom(this->opInfinity(), *this);
  return result;
}

Expression Calculator::expressionMinusInfinity() {
  Expression result;
  result = this->expressionMinusOne() * this->expressionInfinity();
  return result;
}

List<Function> Calculator::OperationHandlers::mergeHandlers() {
  List<Function> result;
  result.addListOnTop(this->handlers);
  result.addListOnTop(this->compositeHandlers);
  return result;
}

std::string Calculator::OperationHandlers::toStringRuleStatusUser() {
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

std::string Calculator::toStringRuleStatusUser() {
  MacroRegisterFunctionWithName("Calculator::toStringRuleStatusUser");
  std::stringstream out;
  for (int i = 0; i < this->operations.size(); i ++) {
    if (this->operations.theValues[i].isZeroPointer()) {
      continue;
    }
    out << this->operations.theValues[i].getElement().toStringRuleStatusUser();
  }
  return out.str();
}

void Calculator::logTime() {
  int64_t currentMilliseconds = global.getElapsedMilliseconds();
  *this << "<br>" << currentMilliseconds - this->statistics.millisecondsLastLog
  << " ms since last log; " << currentMilliseconds - this->statistics.startTimeEvaluationMilliseconds
  << " ms since start. ";
  this->statistics.millisecondsLastLog = currentMilliseconds;
}

void Calculator::logFunctionWithTime(Function& inputF) {
  this->statistics.totalSubstitutions ++;
  if (!this->flagLogEvaluation) {
    return;
  }
  *this << "<hr>Built-in substitution " << this->statistics.totalSubstitutions
  << ": " << inputF.toStringSummary();
  this->logTime();
  *this << "Rule stack id: "
  << this->RuleStackCacheIndex << ", stack size: " << this->ruleStack.size();
}

const List<Function>* Calculator::getOperationCompositeHandlers(int theOp) {
  if (theOp < 0 || theOp >= this->operations.size()) {
    // Instead of crashing, we may instead return nullptr.
    // TODO(tmilev): document why we are so harsh
    // in the crash message.
    // [note: I no longer remember the orginal rationale, if any].
    global.fatal << "Corrupt atom index: " << theOp << global.fatal;
  }
  if (this->operations.theValues[theOp].isZeroPointer()) {
    return nullptr;
  }
  return &this->operations.theValues[theOp].getElementConst().compositeHandlers;
}

const List<Function>* Calculator::getOperationHandlers(int theOp) {
  if (theOp < 0 || theOp >= this->operations.size()) {
    // Instead of crashing, we may instead return nullptr.
    // TODO(tmilev): document why we are so harsh
    // in the crash message.
    // [note: I no longer remember the original rationale, if any].
    global.fatal << "Corrupt atom index: " << theOp << global.fatal;
  }
  if (this->operations.theValues[theOp].isZeroPointer()) {
    return nullptr;
  }
  return &this->operations.theValues[theOp].getElementConst().handlers;
}

bool Calculator::outerStandardCompositeHandler(
  Calculator& calculator,
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
  const List<Function>* theHandlers = calculator.getOperationCompositeHandlers(functionNameNode[0].theData);
  if (theHandlers == nullptr) {
    return false;
  }
  for (int i = 0; i < theHandlers->size; i ++) {
    Function& currentHandler = (*theHandlers)[i];
    if (currentHandler.shouldBeApplied(opIndexParentIfAvailable)) {
      if (currentHandler.apply(
        calculator, input, output, opIndexParentIfAvailable, outputHandler
      )) {
        calculator.logFunctionWithTime(currentHandler);
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

bool Function::apply(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int opIndexParentIfAvailable,
  Function** outputHandler
) {
  if (!this->shouldBeApplied(opIndexParentIfAvailable)) {
    return false;
  }
  if (this->theFunction == nullptr) {
    global.fatal << "Attempt to apply non-initialized function. " << global.fatal;
  }
  if (!this->options.flagIsInner) {
    if (this->theFunction(calculator, input, output)) {
      if (output != input) {
        output.checkConsistency();
        calculator.logFunctionWithTime(*this);
        if (outputHandler != nullptr) {
          *outputHandler = this;
        }
        return true;
      }
    }
    return false;
  }
  if (this->inputFitsMyInnerType(input)) {
    if (this->theFunction(calculator, input, output)) {
      output.checkConsistency();
      calculator.logFunctionWithTime(*this);
      if (outputHandler != nullptr) {
        *outputHandler = this;
      }
      return true;
    }
  }
  return false;
}

bool Calculator::outerStandardHandler(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int opIndexParentIfAvailable,
  Function** outputHandler
) {
  const Expression& functionNameNode = input[0];
  int operationIndex = - 1;
  if (!functionNameNode.isOperation(operationIndex)) {
    return false;
  }
  if (calculator.operations.theValues[operationIndex].isZeroPointer()) {
    return false;
  }
  const List<Function>& handlers = calculator.operations.theValues[operationIndex].getElement().handlers;
  for (int i = 0; i < handlers.size; i ++) {
    Function& currentFunction = handlers[i];
    if (currentFunction.apply(
      calculator, input, output, opIndexParentIfAvailable, outputHandler
    )) {
      return true;
    }
  }
  return false;
}

bool Calculator::outerStandardFunction(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int opIndexParentIfAvailable,
  Function** outputHandler
) {
  MacroRegisterFunctionWithName("Calculator::outerStandardFunction");
  RecursionDepthCounter theCounter(&calculator.recursionDepth);
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isList()) {
    return false;
  }
  if (calculator.outerStandardCompositeHandler(
    calculator, input, output, opIndexParentIfAvailable, outputHandler
  )) {
    return true;
  }
  if (calculator.outerStandardHandler(
    calculator, input, output, opIndexParentIfAvailable, outputHandler
  )) {
    return true;
  }
  return false;
}

bool Calculator::expressionMatchesPattern(
  const Expression& thePattern,
  const Expression& input,
  MapList<Expression, Expression>& matchedExpressions,
  std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("Calculator::expressionMatchesPattern");
  RecursionDepthCounter recursionCounter(&this->recursionDepth);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (!(thePattern.owner == this && input.owner == this)) {
    global.fatal << "This is a programming error. "
    << "Either a pattern or an input has a wrongly  initialized owner: the pattern is "
    << thePattern.toString() << " and the input is "
    << input.toString() << ". The error is certainly in the preceding code; here "
    << "is a stack trace, however beware that the error "
    << "might be in code preceding the stack loading. "
    << global.fatal;
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << " <hr> current input: " << input.toString() << "<br>current pattern: " << thePattern.toString();
    *commentsGeneral << "<br> current matched expressions: " << matchedExpressions.toStringHtml();
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (this->recursionDepth>this->maximumRecursionDepth) {
    std::stringstream out;
    out << "Max recursion depth of " << this->maximumRecursionDepth << " exceeded whlie trying to match expression pattern "
    << thePattern.toString() << " onto expression " << input.toString();
    this->evaluationErrors.addOnTop(out.str());
    return false;
  }
  int opVarB = this->opBind();
  if (thePattern.isListStartingWithAtom(opVarB)) {
    if (!matchedExpressions.contains(thePattern)) {
      matchedExpressions.setKeyValue(thePattern, input);
    }
    if (matchedExpressions.getValueCreate(thePattern) != input) {
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
    if (!(this->expressionMatchesPattern(thePattern[i], input[i], matchedExpressions, commentsGeneral))) {
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
      if (!(this->expressionMatchesPattern(
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

void StateMaintainerCalculator::addRule(const Expression& theRule) {
  if (this->owner == nullptr) {
    global.fatal << "StackMaintainerCalculator has zero owner. " << global.fatal;
  }
  this->owner->ruleStack.addChildOnTop(theRule);
  std::string currentRule;
  if (
    theRule.startsWith(this->owner->opRulesOn()) ||
    theRule.startsWith(this->owner->opRulesOff())
  ) {
    for (int j = 1; j < theRule.size(); j ++) {
      if (!theRule[j].isOfType(&currentRule)) {
        continue;
      }
      if (!this->owner->namedRules.contains(currentRule)) {
        continue;
      }
      this->owner->getFunctionHandlerFromNamedRule(currentRule).options.disabledByUser =
      theRule.startsWith(this->owner->opRulesOff());
    }
  }
  this->owner->RuleStackCacheIndex = this->owner->cachedRuleStacks.getIndex(this->owner->ruleStack);
  if (this->owner->RuleStackCacheIndex == - 1) {
    if (this->owner->cachedRuleStacks.size < this->owner->MaxCachedExpressionPerRuleStack) {
      this->owner->RuleStackCacheIndex = this->owner->cachedRuleStacks.size;
      this->owner->cachedRuleStacks.addOnTop(this->owner->ruleStack);
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
  this->startingRuleStackSize = inputBoss.ruleStack.size();
}

StateMaintainerCalculator::~StateMaintainerCalculator() {
  if (this->owner == nullptr) {
    return;
  }
  Expression& theRuleStack = this->owner->ruleStack;
  std::string currentRuleName;
  bool shouldUpdateRules = false;
  for (int i = this->startingRuleStackSize; i < theRuleStack.size(); i ++) {
    if (
      theRuleStack[i].startsWith(this->owner->opRulesOn()) ||
      theRuleStack[i].startsWith(this->owner->opRulesOff())
    ) {
      for (int j = 1; j < theRuleStack[i].size(); j ++) {
        if (!theRuleStack[i][j].isOfType<std::string>(&currentRuleName)) {
          continue;
        }
        if (!this->owner->namedRules.contains(currentRuleName)) {
          continue;
        }
        Function& currentRule = this->owner->getFunctionHandlerFromNamedRule(currentRuleName);
        currentRule.options.disabledByUser = currentRule.options.disabledByUserDefault;
        shouldUpdateRules = true;
      }
    }
  }
  if (shouldUpdateRules) {
    for (int i = 0; i < this->startingRuleStackSize; i ++) {
      if (theRuleStack[i].startsWith(this->owner->opRulesOn())) {
        for (int j = 1; j < theRuleStack[i].size(); j ++) {
          Function& currentFun = this->owner->getFunctionHandlerFromNamedRule(
            theRuleStack[i][j].getValue<std::string>()
          );
          currentFun.options.disabledByUser = false;
        }
      } else if (theRuleStack[i].startsWith(this->owner->opRulesOff())) {
        for (int j = 1; j < theRuleStack[i].size(); j ++) {
          Function& currentFun = this->owner->getFunctionHandlerFromNamedRule(
            theRuleStack[i][j].getValue<std::string>()
          );
          currentFun.options.disabledByUser = true;
        }
      }
    }
  }
  this->owner->RuleStackCacheIndex = this->startingRuleStackIndex;
  this->owner->ruleStack.children.setSize(this->startingRuleStackSize);
  this->owner = nullptr;
}

Expression Calculator::getNewBoundVariable() {
  Expression result(*this);
  result.addChildAtomOnTop(this->opBind());
  result.addChildOnTop(this->getNewAtom());
  return result;
}

Expression Calculator::getNewAtom() {
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

bool Calculator::accountRule(
  const Expression& ruleE, StateMaintainerCalculator& theRuleStackMaintainer
) {
  MacroRegisterFunctionWithName("Calculator::accountRule");
  RecursionDepthCounter theRecursionCounter(&this->recursionDepth);
  if (this->recursionDepth > this->maximumRecursionDepth) {
    return false;
  }
  if (ruleE.isCalculatorStatusChanger()) {
    theRuleStackMaintainer.addRule(ruleE);
  }
  if (!ruleE.isListStartingWithAtom(this->opCommandEnclosure())) {
    return true;
  }
  if (ruleE.size() <= 1) {
    return true;
  }
  if (!ruleE[1].startsWith(this->opEndStatement())) {
    return this->accountRule(ruleE[1], theRuleStackMaintainer);
  }
  for (int i = 1; i < ruleE[1].size(); i ++) {
    if (!this->accountRule(ruleE[1][i], theRuleStackMaintainer)) {
      return false;
    }
  }
  return true;
}

bool Calculator::evaluateExpression(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::evaluateExpression");
  bool notUsed = false;
  return calculator.evaluateExpression(calculator, input, output, notUsed, - 1, nullptr);
}

bool Calculator::isTimedOut() {
  if (global.millisecondsMaxComputation <= 0) {
    return false;
  }
  int64_t elapsedMilliseconds = global.getElapsedMilliseconds();
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
  this->output = nullptr;
}

void Calculator::EvaluateLoop::accountHistoryChildTransformation(
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
  indexE.assignValue(childIndex, *this->owner);
  incomingHistory.makeXOX(
    *this->owner,
    this->owner->opExpressionHistorySetChild(),
    indexE,
    childHistory
  );
  this->history->addChildOnTop(incomingHistory);
}

void Calculator::EvaluateLoop::accountHistory(Function* handler, const std::string& info) {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::accountHistory");
  this->checkInitialization();
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
    if (lastHistory[1] == *(this->output)) {
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
  extraInformation.assignValue(description.str(), *this->owner);
  incomingHistory.makeXOX(
    *this->owner,
    this->owner->opExpressionHistorySet(),
    *(this->output),
    extraInformation
  );
  incomingHistory.checkConsistency();
  this->history->addChildOnTop(incomingHistory);
}

bool Calculator::EvaluateLoop::setOutput(
  const Expression& input, Function* handler, const std::string& info
) {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::setOutput");
  if (this->output == nullptr) {
    global.fatal << "Non-initialized evaluation loop. " << global.fatal;
  }
  *(this->output) = input;
  this->accountHistory(handler, info);
  return true;
}

void Calculator::EvaluateLoop::initializeOneRun() {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::initializeOneRun");
  this->numberOfTransformations ++;
  std::string atomValue;
  if (this->output->isOperation(&atomValue)) {
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
        this->indexInCache, this->owner->expressionMinusOne()
      );
    }
    this->indexInCache = - 1;
  }
  if (this->indexInCache != - 1) {
    this->owner->imagesCachedExpressions[indexInCache] = *(this->output);
  }
}

bool Calculator::EvaluateLoop::outputHasErrors() {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::outputHasErrors");
  if (this->owner->isTimedOut()) {
    return true;
  }
  if (
    this->numberOfTransformations <=
    this->owner->maximumAlgebraicTransformationsPerExpression
  ) {
    return false;
  }
  if (!this->owner->flagMaxTransformationsErrorEncountered) {
    std::stringstream out;
    out << " While simplifying "
    << this->output->toString(&this->owner->formatVisibleStrings)
    << "<br>maximum number of algebraic transformations of "
    << this->owner->maximumAlgebraicTransformationsPerExpression << " exceeded.";
    this->output->makeError(out.str(), *this->owner);
    this->reductionOccurred = true;
    this->owner->flagAbortComputationASAP = true;
    this->owner->flagMaxTransformationsErrorEncountered = true;
  }
  return true;
}

void Calculator::EvaluateLoop::reportChildEvaluation(Expression& output, int childIndex) {
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
      reportStream << "<br>" << StringRoutines::stringShortenInsertDots(
        output[j].toString(), 100
      );
   }
  }
  reportStream << "<br>Evaluating:<br><b>"
  << StringRoutines::stringShortenInsertDots(output[childIndex].toString(), 100) << "</b>";
  theReport.report(reportStream.str());
}

bool Calculator::EvaluateLoop::evaluateChildren(
  StateMaintainerCalculator& maintainRuleStack
) {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::evaluateChildren");
  if (this->output->isFrozen()) {
    return true;
  }
  int indexOp = - 1;
  if (this->output->size() > 0) {
    if ((*this->output)[0].isAtom()) {
      indexOp = (*this->output)[0].theData;
    }
  }
  Expression childEvaluation, historyContainer;
  Expression* historyChild = nullptr;
  if (this->history != nullptr) {
    historyChild = &historyContainer;
  }
  for (int i = 0; i < this->output->size(); i ++) {
    if (i > 0) {
      this->reportChildEvaluation(*this->output, i);
    }
    bool childIsNonCacheable = false;
    if (this->history != nullptr) {
      this->currentChild = (*this->output)[i];
    }
    if (this->owner->evaluateExpression(
      *this->owner,
      (*this->output)[i],
      childEvaluation,
      childIsNonCacheable,
      indexOp,
      historyChild
    )) {
      this->output->setChild(i, childEvaluation);
    }
    this->accountHistoryChildTransformation(childEvaluation, historyContainer, i);
    // If the child is non-cache-able, so is the current one.
    // Once evaluation has passed through a non-cacheable expression,
    // our expression is no longer cache-able.
    if (childIsNonCacheable) {
      this->flagIsNonCacheable = true;
    }
    if ((*this->output)[i].isError()) {
      this->owner->flagAbortComputationASAP = true;
      return false;
    }
    if (this->output->startsWith(this->owner->opEndStatement())) {
      if (!this->owner->accountRule((*this->output)[i], maintainRuleStack)) {
        std::stringstream out;
        out
        << "Failed to account rule: " << (*this->output)[i].toString()
        << ". Most likely the cause is too deeply nested recursion. ";
        this->output->makeError(out.str(), *this->owner);
        this->owner->flagAbortComputationASAP = true;
      }
    }
    if (this->owner->flagAbortComputationASAP) {
      return false;
    }
  }
  return true;
}

bool Calculator::EvaluateLoop::userDefinedEvaluation() {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::userDefinedEvaluation");
  Expression beforepatternMatch, afterpatternMatch;
  for (
    int i = 0;
    i < this->owner->ruleStack.size() && !this->owner->flagAbortComputationASAP;
    i ++
  ) {
    const Expression& currentPattern = this->owner->ruleStack[i];
    this->owner->totalPatternMatchesPerformed ++;
    if (this->owner->flagLogEvaluation) {
      beforepatternMatch = *this->output;
    }
    MapList<Expression, Expression> bufferPairs;
    std::stringstream* theLog = this->owner->flagLogpatternMatching ? &this->owner->comments : nullptr;
    afterpatternMatch = *(this->output);
    if (this->owner->processOneExpressionOnePatternOneSub(
      currentPattern, afterpatternMatch, bufferPairs, theLog
    )) {
      std::stringstream substitutionComment;
      if (this->history == nullptr) {
        substitutionComment << "User-defined substition: " << currentPattern.toString();
      }
      this->setOutput(afterpatternMatch, nullptr, substitutionComment.str());
      this->reductionOccurred = true;
      if (this->owner->flagLogEvaluation) {
        *this->owner
        << "<hr>Rule cache index: " << this->owner->RuleStackCacheIndex
        << "<br>Rule: " << currentPattern.toString() << "<br>"
        << HtmlRoutines::getMathSpanPure(beforepatternMatch.toString())
        << " -> " << HtmlRoutines::getMathSpanPure(this->output->toString());
      }
      return true;
    }
  }
  return false;
}

bool Calculator::EvaluateLoop::checkInitialization() {
  if (this->owner == nullptr || this->output == nullptr) {
    global.fatal << "Non-initialized evaluation loop. " << global.fatal;
  }
  return true;
}

bool Calculator::EvaluateLoop::builtInEvaluation() {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::builtInEvaluation");
  this->checkInitialization();
  Expression result;
  Function* handlerContainer = nullptr;
  if (!this->owner->outerStandardFunction(
    *(this->owner), *(this->output), result, this->opIndexParent, &handlerContainer
  )) {
    return false;
  }
  this->reductionOccurred = true;
  if (this->owner->flagLogEvaluation) {
    *(this->owner) << "<br>Rule context identifier: "
    << this->owner->RuleStackCacheIndex
    << "<br>" << HtmlRoutines::getMathMouseHover(this->output->toString())
    << " -> " << HtmlRoutines::getMathMouseHover(result.toString());
  }
  return this->setOutput(result, handlerContainer, "");
}

bool Calculator::EvaluateLoop::reduceOnce() {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::reduceOnce");
  StateMaintainerCalculator maintainRuleStack(*(this->owner));
  this->checkInitialization();
  this->initializeOneRun();
  if (this->outputHasErrors()) {
    return false;
  }
  if (!this->evaluateChildren(maintainRuleStack)) {
    return false;
  }
  if (this->owner->flagAbortComputationASAP) {
    return false;
  }
  if (this->builtInEvaluation()) {
    return true;
  }
  if (this->userDefinedEvaluation()) {
    return true;
  }
  return false;
}

void Calculator::EvaluateLoop::lookUpCache() {
  this->owner->evaluatedExpressionsStack.addOnTop(*(this->output));
  Expression theExpressionWithContext;
  theExpressionWithContext.reset(*this->owner, 3);
  theExpressionWithContext.addChildAtomOnTop(this->owner->opSequence());
  theExpressionWithContext.addChildValueOnTop(this->owner->RuleStackCacheIndex);
  theExpressionWithContext.addChildOnTop(*(this->output));
  this->indexInCache = this->owner->cachedExpressions.getIndex(theExpressionWithContext);
  if (this->indexInCache != - 1) {
    if (this->owner->flagLogCache) {
      *this->owner << "<hr>Cache hit with state identifier "
      << this->owner->RuleStackCacheIndex << ": "
      << this->output->toString() << " -> "
      << this->owner->imagesCachedExpressions[this->indexInCache].toString();
    }
    std::stringstream comment;
    if (this->history != nullptr) {
      comment << "Previously computed that \\("
      << this->output->toString() << " = "
      << this->owner->imagesCachedExpressions[this->indexInCache].toString()
      << "\\)";
    }
    this->setOutput(this->owner->imagesCachedExpressions[this->indexInCache], nullptr, comment.str());
    return;
  }
  if (
    this->owner->cachedExpressions.size > this->owner->MaxCachedExpressionPerRuleStack ||
    this->output->isBuiltInType() ||
    this->output->isBuiltInAtom()
  ) {
    return;
  }
  this->owner->cachedExpressions.addOnTop(theExpressionWithContext);
  this->indexInCache = this->owner->cachedExpressions.size - 1;
  this->owner->imagesCachedExpressions.setSize(this->indexInCache + 1);
  this->owner->imagesCachedExpressions.lastObject()->makeError("Error: not computed yet.", *this->owner);
}

bool Calculator::evaluateExpression(
  Calculator& calculator,
  const Expression& input,
  Expression& outpuT,
  bool& outputIsNonCacheable,
  int opIndexParentIfAvailable,
  Expression* outputHistory
) {
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  MacroRegisterFunctionWithName("Calculator::evaluateExpression");
  calculator.statistics.expressionsEvaluated ++;
  calculator.statistics.callsSinceReport ++;
  Calculator::EvaluateLoop state(calculator);
  state.output = &outpuT;
  state.history = outputHistory;
  if (state.history != nullptr) {
    state.history->reset(calculator);
  }
  state.opIndexParent = opIndexParentIfAvailable;
  if (
    calculator.statistics.callsSinceReport >=
    calculator.statistics.maximumCallsBeforeReportGeneration
  ) {
    calculator.statistics.callsSinceReport = 0;
    std::stringstream reportStream;
    reportStream << "Evaluating: " << input.toString();
    state.theReport.report(reportStream.str());
  }
  if (calculator.flagLogFullTreeCrunching && calculator.recursionDepth < 3) {
    calculator << "<br>";
    for (int i = 0; i < calculator.recursionDepth; i ++) {
      calculator << "&nbsp&nbsp&nbsp&nbsp";
    }
    calculator << "Evaluating " << input.lispify()
    << " with rule stack cache index "
    << calculator.RuleStackCacheIndex; // << this->ruleStack.toString();
  }
  if (calculator.recursionDepthExceededHandleRoughly()) {
    return calculator << " Evaluating expression: " << input.toString() << " aborted. ";
  }
  state.setOutput(input, nullptr, "");
  if (state.output->isError()) {
    calculator.flagAbortComputationASAP = true;
    return true;
  }
  if (calculator.evaluatedExpressionsStack.contains(input)) {
    std::stringstream errorStream;
    errorStream << "I think I have detected an infinite cycle: I am asked to reduce "
    << input.toString()
    << " but I have already seen that expression in the expression stack. ";
    calculator.flagAbortComputationASAP = true;
    Expression errorE;
    errorE.makeError(errorStream.str(), calculator);
    return state.setOutput(errorE, nullptr, "Error");
  }
  //bool logEvaluationStepsRequested = calculator.logEvaluationSteps.size > 0;
  state.lookUpCache();
  // reduction phase:
  //////////////////////////////////
  // evaluateExpression is called recursively
  // inside state.evaluateChildren
  // inside state.reduceOnce.
  while (state.reduceOnce()) {
  }
  outputIsNonCacheable = state.flagIsNonCacheable;
  calculator.evaluatedExpressionsStack.removeLastObject();
  if (calculator.flagLogFullTreeCrunching && calculator.recursionDepth < 3) {
    calculator << "<br>";
    for (int i = 0; i < calculator.recursionDepth; i ++) {
      calculator << "&nbsp&nbsp&nbsp&nbsp";
    }
    calculator << "to get: " << state.output->lispify();
  }
  return true;
}

Expression* Calculator::patternMatch(
  const Expression& thePattern,
  Expression& theExpression,
  MapList<Expression, Expression>& bufferPairs,
  const Expression* condition,
  std::stringstream* theLog
) {
  MacroRegisterFunctionWithName("Calculator::patternMatch");
  RecursionDepthCounter recursionCounter(&this->recursionDepth);
  if (this->recursionDepth >= this->maximumRecursionDepth) {
    std::stringstream out;
    out << "Error: while trying to evaluate expression, "
    << "the maximum recursion depth of "
    << this->maximumRecursionDepth << " was exceeded";
    theExpression.makeError(out.str(), *this);
    return nullptr;
  }
  thePattern.checkInitialization();
  theExpression.checkInitialization();
  if (!this->expressionMatchesPattern(
    thePattern, theExpression, bufferPairs, theLog
  )) {
    return nullptr;
  }
  if (theLog != nullptr) {
    (*theLog) << "<hr>found pattern: " << theExpression.toString() << " -> "
    << thePattern.toString() << " with " << bufferPairs.toStringHtml();
  }
  if (condition == nullptr) {
    return &theExpression;
  }
  Expression tempExp = *condition;
  tempExp.checkInitialization();
  if (theLog != nullptr) {
    (*theLog) << "<hr>Specializing condition pattern: " << tempExp.toString();
  }
  this->specializeBoundVariables(tempExp, bufferPairs);
  tempExp.checkInitialization();
  if (theLog != nullptr) {
    (*theLog) << "<hr>Specialized condition: " << tempExp.toString() << "; evaluating...";
  }
  Expression conditionResult;
  this->evaluateExpression(*this, tempExp, conditionResult);
  if (theLog != nullptr) {
    (*theLog) << "<hr>The evaluated specialized condition: " << conditionResult.toString()
    << "; evaluating...";
  }
  if (conditionResult.isEqualToOne()) {
    return &theExpression;
  }
  return nullptr;
}

void Calculator::specializeBoundVariables(Expression& toBeSubbedIn, MapList<Expression, Expression>& matchedPairs) {
  MacroRegisterFunctionWithName("Calculator::specializeBoundVariables");
  RecursionDepthCounter recursionCounter(&this->recursionDepth);
  if (toBeSubbedIn.isListOfTwoAtomsStartingWith(this->opBind())) {
    if (matchedPairs.contains(toBeSubbedIn)) {
      toBeSubbedIn = matchedPairs.getValueCreate(toBeSubbedIn);
      //this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+ 1, MaxRecursionDepth);
      return;
    }
  }
  Expression subbedE;
  for (int i = 0; i < toBeSubbedIn.size(); i ++) {
    subbedE = toBeSubbedIn[i];
    this->specializeBoundVariables(subbedE, matchedPairs);
    toBeSubbedIn.setChild(i, subbedE);
  }
//  this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+ 1, MaxRecursionDepth);
}

bool Calculator::processOneExpressionOnePatternOneSub(
  const Expression& thePattern,
  Expression& theExpression,
  MapList<Expression, Expression>& bufferPairs,
  std::stringstream* theLog
) {
  MacroRegisterFunctionWithName("Calculator::processOneExpressionOnePatternOneSub");
  RecursionDepthCounter recursionCounter(&this->recursionDepth);
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
  theExpression.checkInitialization();
  const Expression& currentPattern = thePattern[1];
  const Expression* theCondition = nullptr;
  bool isConditionalDefine = thePattern.startsWith(this->opDefineConditional(), 4);
  if (isConditionalDefine) {
    theCondition = &thePattern[2];
  }
  Expression* toBeSubbed = this->patternMatch(
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
  this->specializeBoundVariables(*toBeSubbed, bufferPairs);
  return true;
}

bool Calculator::parse(const std::string& input, Expression& output) {
  List<SyntacticElement> syntacticSoup, syntacticStack;
  return this->parseAndExtractExpressions(input, output, syntacticSoup, syntacticStack, nullptr);
}

bool Calculator::parseAndExtractExpressions(
  const std::string& input,
  Expression& output,
  List<SyntacticElement>& outputSynSoup,
  List<SyntacticElement>& outputSynStack,
  std::string* outputSynErrors
) {
  MacroRegisterFunctionWithName("Calculator::parseAndExtractExpressions");
  this->currentSyntacticStack = &outputSynStack;
  this->currrentSyntacticSoup = &outputSynSoup;
  this->parseFillDictionary(input);
  bool result = this->extractExpressions(output, outputSynErrors);
  this->currentSyntacticStack = &this->syntacticStacK;
  this->currrentSyntacticSoup = &this->syntacticSoup;
  return result;
}

void Calculator::EvaluationStatistics::initialize() {
  this->startTimeEvaluationMilliseconds      = global.getElapsedMilliseconds();
  this->millisecondsLastLog                  = this->startTimeEvaluationMilliseconds;
  this->numberOfListsStart                   = static_cast<signed>( GlobalStatistics::numListsCreated    );
  this->numberListResizesStart               = static_cast<signed>( GlobalStatistics::numListResizesTotal);
  this->numberHashResizesStart               = static_cast<signed>( GlobalStatistics::numHashResizes     );
  this->numberOfSmallAdditionsStart          = static_cast<signed>( Rational::totalSmallAdditions         );
  this->numberOfSmallMultiplicationsStart    = static_cast<signed>( Rational::totalSmallMultiplications   );
  this->numberOfSmallGreatestCommonDivisorsStart           = static_cast<signed>( Rational::totalSmallGreatestCommonDivisors          );
  this->numberOfLargeAdditionsStart          = static_cast<signed>( Rational::totalLargeAdditions         );
  this->numberOfLargeMultiplicationsStart    = static_cast<signed>( Rational::totalLargeMultiplications   );
  this->numberOfLargeGreatestCommonDivisorsStart           = static_cast<signed>( Rational::totalLargeGreatestCommonDivisors          );
}

void Calculator::evaluate(const std::string& theInput) {
  MacroRegisterFunctionWithName("Calculator::evaluate");
  this->statistics.initialize();
  this->inputString = theInput;
  this->parseAndExtractExpressions(
    theInput, this->theProgramExpression, this->syntacticSoup, this->syntacticStacK, &this->syntaxErrors
  );
  this->evaluateCommands();
}

void Calculator::evaluateCommands() {
  MacroRegisterFunctionWithName("Calculator::evaluateCommands");
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
  this->evaluateExpression(*this, this->theProgramExpression, this->theProgramExpression);
  if (this->recursionDepth != 0) {
    global.fatal << "This is a programming error: the starting recursion "
    << "depth before evaluation was 0, but after evaluation it is "
    << this->recursionDepth << "." << global.fatal;
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
    std::string badCharsString = this->toStringIsCorrectAsciiCalculatorString(this->inputString);
    if (badCharsString != "") {
      out << badCharsString << "<hr>";
      this->outputJS[WebAPI::result::badInput] = badCharsString;
    }
    this->theObjectContainer.resetSliders();
    this->theObjectContainer.resetPlots();
    std::string javascriptString = this->theObjectContainer.toStringJavascriptForUserInputBoxes();
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
    std::string badCharsString = this->toStringIsCorrectAsciiCalculatorString(this->inputString);
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
