// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "string_constants.h"

JSData Calculator::OperationHandlers::toJSON() {
  JSData result;
  JSData currentFunctionListDirect;
  currentFunctionListDirect.elementType = JSData::token::tokenArray;
  for (int i = 0; i < this->handlers.size; i ++) {
    Function& currentHandler = this->handlers[i];
    currentFunctionListDirect.listObjects.addOnTop(currentHandler.toJSON());
  }
  JSData currentFunctionListComposite;
  currentFunctionListComposite.elementType = JSData::token::tokenArray;
  for (int i = 0; i < this->compositeHandlers.size; i ++) {
    Function& currentHandler = this->compositeHandlers[i];
    currentFunctionListComposite.listObjects.addOnTop(currentHandler.toJSON());
  }
  result["regular"] = currentFunctionListDirect;
  result["composite"] = currentFunctionListComposite;
  return result;
}

Calculator::Examples::Examples() {
  this->owner = nullptr;
}

JSData Calculator::Examples::toJSONFunctionHandlers() {
  MacroRegisterFunctionWithName("Calculator::toJSONFunctionHandlers");
  JSData output;
  output.elementType = JSData::token::tokenObject;
  MapReferences<std::string, MemorySaving<OperationHandlers>, HashFunctions::hashFunction>&
  operations = this->owner->operations;
  for (int i = 0; i < operations.size(); i ++) {
    if (operations.values[i].isZeroPointer()) {
      continue;
    }
    const std::string& operationName = operations.keys[i];
    Calculator::OperationHandlers& handlers = operations.values[i].getElement();
    output[operationName] = handlers.toJSON();
  }
  return output;
}

std::string Calculator::Examples::escape(const std::string& atom) {
  if (this->toBeEscaped.size == 0) {
    this->toBeEscaped = List<std::string>({
      "\\", "`", "*", "_",
      "{", "}", "[", "]", "<", ">", "(", ")",
      "#", "+", "-", ".", "!", "|"
    });
  }
  if (!this->toBeEscaped.contains(atom)) {
    return atom;
  }
  return "\\" + atom;
}

std::string Calculator::Examples::getExamplesReadmeFragment() {
  MacroRegisterFunctionWithName("Calculator::getExamplesReadmeFragment");
  std::stringstream out;

  MapReferences<std::string, MemorySaving<OperationHandlers>, HashFunctions::hashFunction>&
  operations = this->owner->operations;
  for (int i = 0; i < operations.size(); i ++) {
    if (operations.values[i].isZeroPointer()) {
      continue;
    }
    std::string atomEscaped = this->escape(operations.keys[i]);
    Calculator::OperationHandlers& handlers = operations.values[i].getElement();
    int totalHandlers = handlers.handlers.size + handlers.compositeHandlers.size;
    if (totalHandlers > 1) {
      out << "\n\nOperator or function " << atomEscaped
      << " is overloaded with "
      << totalHandlers << " total handlers.";
    }
    for (int j = 0; j < handlers.handlers.size; j ++ ) {
      out << "\n\n" << this->toStringOneOperationHandler(atomEscaped, false, handlers.handlers[j]);
    }
    for (int j = 0; j < handlers.compositeHandlers.size; j ++ ) {
      out << "\n\n" << this->toStringOneOperationHandler(atomEscaped, true, handlers.compositeHandlers[j]);
    }
  }
  return out.str();
}

std::string Calculator::Examples::toStringOneOperationHandler(
  const std::string& escapedAtom,
  bool isComposite,
  const Function& function
) {
  std::stringstream out;
  out << "*" << escapedAtom << "*";
  if (isComposite) {
    out << " (_composite_)";
  }
  out << " [" << function.calculatorIdentifier << "]";
  if (function.additionalIdentifier != "") {
    out << " {" << function.additionalIdentifier << "}. ";
  }
  if (function.options.adminOnly) {
    out << "(admin only) ";
  }
  if (!function.options.visible) {
    out << "(invisible) ";
  }
  out << "\n";
  out << "[Example](" << "https://calculator-algebra.org/"
  << WebAPI::app
  << HtmlRoutines::getCalculatorComputationURL(function.example) << ")\n";
  out << "```\n";
  out << function.example;
  out << "\n```";
  out << "\n";
  out << function.description;
  return out.str();
}

bool Calculator::Examples::writeExamplesReadme() {
  std::stringstream out;
  std::string readmeTemplate;
  if (!FileOperations::loadFileToStringVirtual(
    "examples/readme_template.md", readmeTemplate, false, nullptr
  )) {
    global << Logger::red << "Failed to read readme template." << Logger::endL;
    return false;
  }
  std::string examples = this->getExamplesReadmeFragment();
  StringRoutines::replaceOnce(readmeTemplate,
    "${content-inserted-by-calculator-in-Calculator::Examples::writeExamplesReadme}",
    examples
  );
  return FileOperations::writeFileVirual("examples/README.md", readmeTemplate, nullptr);
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


Expression Calculator::expressionRational(const Rational& input) {
  Expression result;
  result.assignValue(*this, input);
  return result;
}

Expression Calculator::expressionInteger(int input) {
  Expression result;
  result.assignValue(*this, input);
  return result;
}

Expression Calculator::expressionZero() {
  return this->expressionInteger(0);
}

Expression Calculator::expressionOne() {
  return this->expressionInteger(1);
}

Expression Calculator::expressionMinusOne() {
  return this->expressionInteger(- 1);
}

Expression Calculator::expressionFour() {
  return this->expressionInteger(4);
}

Expression Calculator::expressionTwo() {
  return this->expressionInteger(2);
}

Expression Calculator::expressionMinusHalf() {
  Expression result;
  result.assignValue(*this, Rational(- 1, 2));
  return result;
}

Expression Calculator::expressionInfinity() {
  Expression result;
  result.makeAtom(*this, this->opInfinity());
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
    if (this->operations.values[i].isZeroPointer()) {
      continue;
    }
    out << this->operations.values[i].getElement().toStringRuleStatusUser();
  }
  return out.str();
}

void Calculator::logTime(int64_t startTime) {
  int64_t currentMilliseconds = global.getElapsedMilliseconds();
  *this << "<br>" << currentMilliseconds - this->statistics.millisecondsLastLog
  << " ms since last log; " << currentMilliseconds - this->statistics.startTimeEvaluationMilliseconds
  << " ms since start; " << currentMilliseconds - startTime << " ms since last stopwatch. ";
  this->statistics.millisecondsLastLog = currentMilliseconds;
}

void Calculator::logFunctionWithTime(Function& input, int64_t startTime) {
  this->statistics.totalSubstitutions ++;
  if (!this->flagLogEvaluation) {
    return;
  }
  *this << "<hr>Built-in substitution " << this->statistics.totalSubstitutions
  << ": " << input.toStringSummary();
  this->logTime(startTime);
  *this << "Rule stack size: " << this->ruleStack.size();
}

const List<Function>* Calculator::getOperationCompositeHandlers(int operation) {
  if (operation < 0 || operation >= this->operations.size()) {
    // Instead of crashing, we may instead return nullptr.
    // TODO(tmilev): document why we are so harsh
    // in the crash message.
    // [note: I no longer remember the orginal rationale, if any].
    global.fatal << "Corrupt atom index: " << operation << global.fatal;
  }
  if (this->operations.values[operation].isZeroPointer()) {
    return nullptr;
  }
  return &this->operations.values[operation].getElementConst().compositeHandlers;
}

const List<Function>* Calculator::getOperationHandlers(int operation) {
  if (operation < 0 || operation >= this->operations.size()) {
    // Instead of crashing, we may instead return nullptr.
    // TODO(tmilev): document why we are so harsh
    // in the crash message.
    // [note: I no longer remember the original rationale, if any].
    global.fatal << "Corrupt atom index: " << operation << global.fatal;
  }
  if (this->operations.values[operation].isZeroPointer()) {
    return nullptr;
  }
  return &this->operations.values[operation].getElementConst().handlers;
}

bool Calculator::outerStandardCompositeHandler(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int opIndexParentIfAvailable,
  Function** outputHandler
) {
  int64_t start = 0;
  if (calculator.flagLogEvaluation) {
    start = global.getElapsedMilliseconds();
  }
  if (!input.isList()) {
    return false;
  }
  const Expression& functionNameNode = input[0];
  if (!functionNameNode.startsWith()) {
    return false;
  }
  const List<Function>* handlers = calculator.getOperationCompositeHandlers(functionNameNode[0].data);
  if (handlers == nullptr) {
    return false;
  }
  for (int i = 0; i < handlers->size; i ++) {
    Function& currentHandler = (*handlers)[i];
    if (currentHandler.shouldBeApplied(opIndexParentIfAvailable)) {
      if (currentHandler.apply(
        calculator, input, output, opIndexParentIfAvailable, outputHandler
      )) {
        calculator.logFunctionWithTime(currentHandler, start);
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
  int64_t start = 0;
  if (this->owner->flagLogEvaluation) {
    start = global.getElapsedMilliseconds();
  }
  if (!this->shouldBeApplied(opIndexParentIfAvailable)) {
    return false;
  }
  if (this->functionAddress == nullptr) {
    global.fatal << "Attempt to apply non-initialized function. " << global.fatal;
  }
  if (!this->options.flagIsInner) {
    if (this->functionAddress(calculator, input, output)) {
      if (output != input) {
        output.checkConsistency();
        calculator.logFunctionWithTime(*this, start);
        if (outputHandler != nullptr) {
          *outputHandler = this;
        }
        return true;
      }
    }
    return false;
  }
  if (this->inputFitsMyInnerType(input)) {
    if (this->functionAddress(calculator, input, output)) {
      output.checkConsistency();
      calculator.logFunctionWithTime(*this, start);
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
  if (calculator.operations.values[operationIndex].isZeroPointer()) {
    return false;
  }
  const List<Function>& handlers = calculator.operations.values[operationIndex].getElement().handlers;
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
  const Expression& pattern,
  const Expression& input,
  MapList<Expression, Expression>& matchedExpressions,
  std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("Calculator::expressionMatchesPattern");
  RecursionDepthCounter recursionCounter(&this->recursionDepth);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (!(pattern.owner == this && input.owner == this)) {
    global.fatal
    << "Either a pattern or an input has a wrongly  initialized owner: the pattern is "
    << pattern.toString() << " and the input is "
    << input.toString() << ". The error is certainly in the preceding code; here "
    << "is a stack trace, however beware that the error "
    << "might be in code preceding the stack loading. "
    << global.fatal;
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << " <hr> current input: "
    << input.toString() << "<br>current pattern: "
    << pattern.toString();
    *commentsGeneral << "<br> current matched expressions: " << matchedExpressions.toStringHtml();
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (this->recursionDepth>this->maximumRecursionDepth) {
    std::stringstream out;
    out << "Max recursion depth of " << this->maximumRecursionDepth << " exceeded whlie trying to match expression pattern "
    << pattern.toString() << " onto expression " << input.toString();
    this->evaluationErrors.addOnTop(out.str());
    return false;
  }
  int bindOperation = this->opBind();
  if (pattern.isListStartingWithAtom(bindOperation)) {
    if (!matchedExpressions.contains(pattern)) {
      matchedExpressions.setKeyValue(pattern, input);
    }
    if (matchedExpressions.getValueCreateEmpty(pattern) != input) {
      return false;
    }
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "<br><b>Match!</b>";
    }
    return true;
  }
  if (pattern.data != input.data || pattern.size() != input.size()) {
    return false;
  }
  bool isGoodRegularOrder = true;
  int numMatchedExpressionsAtStart = matchedExpressions.size();
  for (int i = 0; i < pattern.size(); i ++) {
    if (!(this->expressionMatchesPattern(pattern[i], input[i], matchedExpressions, commentsGeneral))) {
      if (i == 0) {
        return false;
      }
      isGoodRegularOrder = false;
      break;
    }
  }

  if (!isGoodRegularOrder) {
    if (!input.startsWith(pattern.owner->opPlus())) {
      return false;
    }
    matchedExpressions.values.setSize(numMatchedExpressionsAtStart);
    matchedExpressions.keys.setSize(numMatchedExpressionsAtStart);
    for (int i = 1; i < pattern.size(); i ++) {
      if (!(this->expressionMatchesPattern(
        pattern[i], input[pattern.size() - i], matchedExpressions, commentsGeneral
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

void StateMaintainerCalculator::addRule(const Expression& rule) {
  if (this->owner == nullptr) {
    global.fatal << "StackMaintainerCalculator has zero owner. " << global.fatal;
  }
  this->owner->ruleStack.addChildOnTop(rule);
  this->owner->cachedExpressions.clear();
  std::string currentRule;
  if (
    rule.startsWith(this->owner->opRulesOn()) ||
    rule.startsWith(this->owner->opRulesOff())
  ) {
    for (int j = 1; j < rule.size(); j ++) {
      if (!rule[j].isOfType(&currentRule)) {
        continue;
      }
      if (!this->owner->namedRules.contains(currentRule)) {
        continue;
      }
      this->owner->getFunctionHandlerFromNamedRule(currentRule).options.disabledByUser =
      rule.startsWith(this->owner->opRulesOff());
    }
  }
  if (this->owner->flagLogRules) {
    *this->owner << "<hr>Added rule " << rule.toString();
  }
}

StateMaintainerCalculator::StateMaintainerCalculator(Calculator& inputBoss) {
  this->owner = &inputBoss;
  this->startingRuleStackSize = inputBoss.ruleStack.size();
}

StateMaintainerCalculator::~StateMaintainerCalculator() {
  if (this->owner == nullptr) {
    return;
  }
  Expression& ruleStack = this->owner->ruleStack;
  std::string currentRuleName;
  bool shouldUpdateRules = false;
  for (int i = this->startingRuleStackSize; i < ruleStack.size(); i ++) {
    if (
      ruleStack[i].startsWith(this->owner->opRulesOn()) ||
      ruleStack[i].startsWith(this->owner->opRulesOff())
    ) {
      for (int j = 1; j < ruleStack[i].size(); j ++) {
        if (!ruleStack[i][j].isOfType<std::string>(&currentRuleName)) {
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
      if (ruleStack[i].startsWith(this->owner->opRulesOn())) {
        for (int j = 1; j < ruleStack[i].size(); j ++) {
          Function& currentFun = this->owner->getFunctionHandlerFromNamedRule(
            ruleStack[i][j].getValue<std::string>()
          );
          currentFun.options.disabledByUser = false;
        }
      } else if (ruleStack[i].startsWith(this->owner->opRulesOff())) {
        for (int j = 1; j < ruleStack[i].size(); j ++) {
          Function& currentFun = this->owner->getFunctionHandlerFromNamedRule(
            ruleStack[i][j].getValue<std::string>()
          );
          currentFun.options.disabledByUser = true;
        }
      }
    }
  }
  if (
    this->owner->ruleStack.size() != this->startingRuleStackSize
  ) {
    this->owner->cachedExpressions.clear();
  }
  this->owner->ruleStack.setSize(this->startingRuleStackSize);
  this->owner = nullptr;
}

bool Calculator::approximationsBanned() {
  Function& approximationsDummyHandler = this->getFunctionHandlerFromNamedRule(
    Calculator::Atoms::approximations
  );
  return approximationsDummyHandler.options.disabledByUser;
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
        result.makeAtom(*this, candidate);
        return result;
      }
    }
    atomPrefix = candidate;
  }
}

bool Calculator::accountRule(
  const Expression& ruleE, StateMaintainerCalculator& ruleStackMaintainer
) {
  MacroRegisterFunctionWithName("Calculator::accountRule");
  RecursionDepthCounter theRecursionCounter(&this->recursionDepth);
  if (this->recursionDepth > this->maximumRecursionDepth) {
    return false;
  }
  if (ruleE.isCalculatorStatusChanger()) {
    ruleStackMaintainer.addRule(ruleE);
  }
  if (!ruleE.isListStartingWithAtom(this->opCommandEnclosure())) {
    return true;
  }
  if (ruleE.size() <= 1) {
    return true;
  }
  if (!ruleE[1].startsWith(this->opCommandSequence())) {
    return this->accountRule(ruleE[1], ruleStackMaintainer);
  }
  for (int i = 1; i < ruleE[1].size(); i ++) {
    if (!this->accountRule(ruleE[1][i], ruleStackMaintainer)) {
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
  indexE.assignValueOLD(childIndex, *this->owner);
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
  extraInformation.assignValueOLD(description.str(), *this->owner);
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

Calculator::ExpressionCache::ExpressionCache() {
  this->ruleState = - 1;
  this->flagNonCacheable = false;
  this->flagFinalized = false;
}

void Calculator::EvaluateLoop::accountIntermediateState() {
  MacroRegisterFunctionWithName("Calculator:EvaluateLoop:::accountIntermediateState");
  std::string atomValue;
  if (this->output->isOperation(&atomValue)) {
    if (this->owner->atomsThatMustNotBeCached.contains(atomValue)) {
      this->flagIsNonCacheable = true;
    }
  }
  if (!this->flagIsNonCacheable) {
    this->intermediateOutputs.addOnTop(*this->output);
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
    this->output->assignError(*this->owner, out.str());
    this->reductionOccurred = true;
    this->owner->flagAbortComputationASAP = true;
    this->owner->flagMaxTransformationsErrorEncountered = true;
  }
  return true;
}

void Calculator::EvaluateLoop::reportChildEvaluation(Expression& output, int childIndex) {
  if (!this->report.tickAndWantReport()) {
    return;
  }
  std::stringstream ruleStream, reportStream;
  int ruleCount = 0;
  for (int j = 1; j < childIndex; j ++) {
    if (
      output[j].startsWith(this->owner->opDefine()) ||
      output[j].startsWith(this->owner->opDefineConditional())
    ) {
      ruleStream << "<br>" << StringRoutines::stringShortenInsertDots(
        output[j].toString(), 100
      );
      ruleCount ++;
   }
  }
  if (ruleCount > 0) {
    reportStream << ruleCount << " substitution rules so far: "
    << ruleStream.str() << "<br>";
  }
  reportStream << "Evaluating at recursion depth " << this->owner->recursionDepth << ":<br><b>"
  << StringRoutines::stringShortenInsertDots(output[childIndex].toString(), 100) << "</b>";
  report.report(reportStream.str());
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
      indexOp = (*this->output)[0].data;
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
    if (this->output->startsWith(this->owner->opCommandSequence())) {
      if (!this->owner->accountRule((*this->output)[i], maintainRuleStack)) {
        std::stringstream out;
        out
        << "Failed to account rule: " << (*this->output)[i].toString()
        << ". Most likely the cause is too deeply nested recursion. ";
        this->output->assignError(*this->owner, out.str());
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
    this->owner->statistics.totalPatternMatchesPerformed ++;
    if (this->owner->flagLogEvaluation) {
      beforepatternMatch = *this->output;
    }
    MapList<Expression, Expression> bufferPairs;
    std::stringstream* theLog = this->owner->flagLogpatternMatching ? &this->owner->comments : nullptr;
    afterpatternMatch = *(this->output);
    if (this->owner->processOneExpressionOnePatternOneSubstitution(
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
        << "<hr>Rule: " << currentPattern.toString() << "<br>"
        << HtmlRoutines::getMathNoDisplay(beforepatternMatch.toString())
        << " -> " << HtmlRoutines::getMathNoDisplay(this->output->toString());
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
    *(this->owner) << "<br>" << HtmlRoutines::getMathNoDisplay(this->output->toString())
    << " -> " << HtmlRoutines::getMathNoDisplay(result.toString());
  }
  return this->setOutput(result, handlerContainer, "");
}

bool Calculator::EvaluateLoop::detectLoops() {
  if (!this->intermediateOutputs.contains(*this->output)) {
    return false;
  }
  std::stringstream errorStream;
  errorStream << "Detected 'infinite' substitution cycle: ";
  for (int i = 0; i < this->intermediateOutputs.size; i ++) {
    errorStream << this->intermediateOutputs[i].toString()
    << " -> ";
  }
  errorStream << this->output->toString();
  Expression error;
  error.assignError(*this->owner, errorStream.str());
  this->setOutput(error, nullptr, errorStream.str());
  this->owner->flagAbortComputationASAP = true;
  return true;
}

bool Calculator::EvaluateLoop::reduceOnce() {
  MacroRegisterFunctionWithName("Calculator::EvaluateLoop::reduceOnce");
  this->checkInitialization();
  this->numberOfTransformations ++;
  this->owner->statistics.totalEvaluationLoops ++;
  if (this->detectLoops()) {
    return false;
  }
  this->accountIntermediateState();
  if (this->reduceUsingCache()) {
    return false;
  }
  StateMaintainerCalculator maintainRuleStack(*(this->owner));
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

bool Calculator::EvaluateLoop::reduceUsingCache() {
  ExpressionCache& cache = this->owner->cachedExpressions.getValueCreateEmpty(*this->output);
  if (!cache.flagFinalized || cache.flagNonCacheable) {
    return false;
  }
  if (this->owner->flagLogCache) {
    *this->owner << "<hr>Cache hit at rule stack size: "
    << this->owner->ruleStack.size() << ": "
    << this->output->toString() << " -> "
    << cache.reducesTo.toString();
  }
  std::stringstream comment;
  if (this->history != nullptr) {
    comment << "\\text{Previously~computed~that~} "
    << this->output->toString() << " = "
    << cache.reducesTo.toString();
  }
  this->setOutput(cache.reducesTo, nullptr, comment.str());
  return true;
}

void Calculator::EvaluateLoop::writeCache() {
  for (int i = 0; i < this->intermediateOutputs.size; i ++) {
    Calculator::ExpressionCache& cache =
    this->owner->cachedExpressions.getValueCreateEmpty(
      this->intermediateOutputs[i]
    );
    if (this->flagIsNonCacheable) {
      cache.flagNonCacheable = true;
    } else {
      cache.flagFinalized = true;
      cache.reducesTo = *this->output;
    }
  }
}

bool Calculator::evaluateExpression(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool& outputIsNonCacheable,
  int opIndexParentIfAvailable,
  Expression* outputHistory
) {
  MacroRegisterFunctionWithName("Calculator::evaluateExpression");
  RecursionDepthCounter recursionCounter;
  recursionCounter.initialize(&calculator.recursionDepth);
  calculator.statistics.expressionsEvaluated ++;
  calculator.statistics.callsSinceReport ++;
  Calculator::EvaluateLoop state(calculator);
  state.output = &output;
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
    state.report.report(reportStream.str());
  }
  if (calculator.flagLogFullTreeCrunching && calculator.recursionDepth < 3) {
    calculator << "<br>";
    for (int i = 0; i < calculator.recursionDepth; i ++) {
      calculator << "&nbsp&nbsp&nbsp&nbsp";
    }
    calculator << "Evaluating " << input.lispify()
    << " with rule stack size "
    << calculator.ruleStack.size();
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
    errorE.assignError(calculator, errorStream.str());
    return state.setOutput(errorE, nullptr, "Error");
  }
  // reduction phase:
  //////////////////////////////////
  // evaluateExpression is called recursively
  // inside state.evaluateChildren
  // inside state.reduceOnce
  // inside reduce.
  calculator.reduce(state);
  outputIsNonCacheable = state.flagIsNonCacheable;
  if (calculator.flagLogFullTreeCrunching && calculator.recursionDepth < 3) {
    calculator << "<br>";
    for (int i = 0; i < calculator.recursionDepth; i ++) {
      calculator << "&nbsp&nbsp&nbsp&nbsp";
    }
    calculator << "to get: " << state.output->lispify();
  }
  return true;
}

void Calculator::reduce(Calculator::EvaluateLoop& state) {
  this->evaluatedExpressionsStack.addOnTop(*state.output);
  while (state.reduceOnce()) {
  }
  state.writeCache();
  this->evaluatedExpressionsStack.removeLastObject();
}

Expression* Calculator::patternMatch(
  const Expression& pattern,
  Expression& expression,
  MapList<Expression, Expression>& bufferPairs,
  const Expression* condition,
  std::stringstream* logStream
) {
  MacroRegisterFunctionWithName("Calculator::patternMatch");
  RecursionDepthCounter recursionCounter(&this->recursionDepth);
  if (this->recursionDepth >= this->maximumRecursionDepth) {
    std::stringstream out;
    out << "Error: while trying to evaluate expression, "
    << "the maximum recursion depth of "
    << this->maximumRecursionDepth << " was exceeded";
    expression.assignError(*this, out.str());
    return nullptr;
  }
  pattern.checkInitialization();
  expression.checkInitialization();
  if (!this->expressionMatchesPattern(
    pattern, expression, bufferPairs, logStream
  )) {
    return nullptr;
  }
  if (logStream != nullptr) {
    (*logStream) << "<hr>found pattern: " << expression.toString() << " -> "
    << pattern.toString() << " with " << bufferPairs.toStringHtml();
  }
  if (condition == nullptr) {
    return &expression;
  }
  Expression tempExp = *condition;
  tempExp.checkInitialization();
  if (logStream != nullptr) {
    (*logStream) << "<hr>Specializing condition pattern: " << tempExp.toString();
  }
  this->specializeBoundVariables(tempExp, bufferPairs);
  tempExp.checkInitialization();
  if (logStream != nullptr) {
    (*logStream) << "<hr>Specialized condition: " << tempExp.toString() << "; evaluating...";
  }
  Expression conditionResult;
  this->evaluateExpression(*this, tempExp, conditionResult);
  if (logStream != nullptr) {
    (*logStream) << "<hr>The evaluated specialized condition: " << conditionResult.toString()
    << "; evaluating...";
  }
  if (conditionResult.isEqualToOne()) {
    return &expression;
  }
  return nullptr;
}

void Calculator::specializeBoundVariables(Expression& toBeSubbedIn, MapList<Expression, Expression>& matchedPairs) {
  MacroRegisterFunctionWithName("Calculator::specializeBoundVariables");
  RecursionDepthCounter recursionCounter(&this->recursionDepth);
  if (toBeSubbedIn.isListOfTwoAtomsStartingWith(this->opBind())) {
    if (matchedPairs.contains(toBeSubbedIn)) {
      toBeSubbedIn = matchedPairs.getValueCreateEmpty(toBeSubbedIn);
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

bool Calculator::processOneExpressionOnePatternOneSubstitution(
  const Expression& pattern,
  Expression& expression,
  MapList<Expression, Expression>& bufferPairs,
  std::stringstream* logStream
) {
  MacroRegisterFunctionWithName("Calculator::processOneExpressionOnePatternOneSub");
  RecursionDepthCounter recursionCounter(&this->recursionDepth);
  if (
    !pattern.startsWith(this->opDefine(), 3) &&
    !pattern.startsWith(this->opDefineConditional(), 4)
  ) {
    return false;
  }
  if (logStream != nullptr) {
    (*logStream) << "<hr>attempting to reduce expression " << expression.toString();
    (*logStream) << " by pattern " << pattern.toString();
  }
  expression.checkInitialization();
  const Expression& currentPattern = pattern[1];
  const Expression* condition = nullptr;
  bool isConditionalDefine = pattern.startsWith(this->opDefineConditional(), 4);
  if (isConditionalDefine) {
    condition = &pattern[2];
  }
  Expression* toBeSubbed = this->patternMatch(
    currentPattern, expression, bufferPairs, condition, logStream
  );
  if (toBeSubbed == nullptr) {
    return false;
  }
  if (logStream != nullptr) {
    *logStream << "<br><b>found a match!</b>";
  }
  if (isConditionalDefine) {
    *toBeSubbed = pattern[3];
  } else {
    *toBeSubbed = pattern[2];
  }
  this->specializeBoundVariables(*toBeSubbed, bufferPairs);
  return true;
}

Expression Calculator::parseOrCrash(
  const std::string& input, bool stripCommandSequence
) {
  Expression result;
  if (!this->parser.parse(input, stripCommandSequence, result)) {
    global.fatal << "Failed to parse: " << input
    << " with a function that does not allow failure."
    << global.fatal;
  }
  return result;
}

void Calculator::EvaluationStatistics::initialize() {
  this->startTimeEvaluationMilliseconds          = global.getElapsedMilliseconds();
  this->millisecondsLastLog                      = this->startTimeEvaluationMilliseconds;
  this->numberOfListsStart                       = static_cast<signed>( GlobalStatistics::numberOfListsCreated         );
  this->numberListResizesStart                   = static_cast<signed>( GlobalStatistics::numberOfListResizesTotal     );
  this->numberHashResizesStart                   = static_cast<signed>( GlobalStatistics::numberOfHashResizes          );
  this->numberOfSmallAdditionsStart              = static_cast<signed>( Rational::totalSmallAdditions             );
  this->numberOfSmallMultiplicationsStart        = static_cast<signed>( Rational::totalSmallMultiplications       );
  this->numberOfSmallGreatestCommonDivisorsStart = static_cast<signed>( Rational::totalSmallGreatestCommonDivisors);
  this->numberOfLargeAdditionsStart              = static_cast<signed>( Rational::totalLargeAdditions             );
  this->numberOfLargeMultiplicationsStart        = static_cast<signed>( Rational::totalLargeMultiplications       );
  this->numberOfLargeGreatestCommonDivisorsStart = static_cast<signed>( Rational::totalLargeGreatestCommonDivisors);
}

void Calculator::evaluate(const std::string& input) {
  MacroRegisterFunctionWithName("Calculator::evaluate");
  this->statistics.initialize();
  this->inputString = input;
  this->parser.parseAndExtractExpressionsDefault(
    input,
    this->programExpression
  );
  this->evaluateCommands();
}

JSData Calculator::solve(const std::string& input) {
  MacroRegisterFunctionWithName("Calculator::solve");
  this->statistics.initialize();
  // this->inputString = input;
  Expression toBeSolved;
  if (!this->parser.parseAndExtractExpressionsDefault(
    input,
    toBeSolved
  )) {
    return this->extractSolution();
  }
  if (toBeSolved.startsWith(this->opCommandSequence(), 2)) {
    toBeSolved = toBeSolved[1];
  }
  this->programExpression.makeOX(*this, this->opSolveJSON(), toBeSolved);
  this->evaluateCommands();
  return this->extractSolution();
}

void Calculator::evaluateCommands() {
  MacroRegisterFunctionWithName("Calculator::evaluateCommands");
  std::stringstream out;
  if (this->parser.syntaxErrors != "") {
    if (!global.flagRunningConsoleRegular) {
      out << "<hr><b>Syntax errors encountered</b><br>";
    } else {
      out << Logger::consoleRed() << "Syntax errors encountered: " << Logger::consoleNormal();
    }
    this->outputJS[WebAPI::result::syntaxErrors] = this->parser.toStringSyntacticStackHTMLSimple();
    out << this->parser.syntaxErrors;
    out << "<hr>";
  }
  Expression startingExpression = this->programExpression;
  this->flagAbortComputationASAP = false;
  this->comments.clear();
  ProgressReport report;
  if (!global.flagRunningConsoleRegular) {
    report.report("Evaluating expressions, current expression stack:\n");
  }
  this->evaluateExpression(*this, startingExpression, this->programExpression);
  if (this->recursionDepth != 0) {
    global.fatal << "The starting recursion "
    << "depth before evaluation was 0, but after evaluation it is "
    << this->recursionDepth << "." << global.fatal;
  }
  global.defaultFormat.getElement().flagMakingExpressionTableWithLatex = true;
  global.defaultFormat.getElement().flagUseLatex = true;
  global.defaultFormat.getElement().flagExpressionNewLineAllowed = true;
  global.defaultFormat.getElement().flagIncludeExtraHtmlDescriptionsInPlots = !this->flagPlotNoControls;
  global.defaultFormat.getElement().flagLatexDetailsInHtml = this->flagWriteLatexPlots;
  global.defaultFormat.getElement().flagExpressionIsFinal = true;
  if (global.flagRunningConsoleRegular) {
    global.defaultFormat.getElement().flagUseQuotes = false;
    global.defaultFormat.getElement().flagExpressionIsFinal = true;
    if (global.programArguments.size > 1) {
      out << "Input: " << Logger::consoleYellow()
      << startingExpression.toString(&global.defaultFormat.getElement()) << std::endl;
    }
    global.defaultFormat.getElement().flagExpressionIsFinal = true;
    this->objectContainer.resetSliders();
    out << Logger::consoleNormal() << "Output: " << Logger::consoleGreen()
    << this->programExpression.toString(&global.defaultFormat.getElement())
    << Logger::consoleNormal() << std::endl;
  } else if (!this->flagDisplayFullExpressionTree) {
    std::string badCharactersString = this->parser.toStringIsCorrectAsciiCalculatorString(this->inputString);
    if (badCharactersString != "") {
      out << badCharactersString << "<hr>";
      this->outputJS[WebAPI::result::badInput] = badCharactersString;
    }
    this->objectContainer.resetSliders();
    this->objectContainer.resetPlots();
    std::string javascriptString = this->objectContainer.toStringJavascriptForUserInputBoxes();
    if (javascriptString != "") {
      this->outputJS["javascriptForUserInputBoxes"] = javascriptString;
    }
    out << javascriptString;
    JSData result;
    result.elementType = JSData::token::tokenObject;
    std::string resultString = this->programExpression.toString(
      &global.defaultFormat.getElement(), &startingExpression, true, &result
    );
    this->outputJS[WebAPI::result::resultLabel] = result;
    out << resultString;
  } else {
    std::string badCharsString = this->parser.toStringIsCorrectAsciiCalculatorString(this->inputString);
    if (badCharsString != "") {
      out << badCharsString << "<hr>";
      this->outputJS[WebAPI::result::badInput] = badCharsString;
    }
    this->objectContainer.resetSliders();
    out << "<hr>Input:<br> " << startingExpression.toStringFull() << "<hr>"
    << "Output:<br>" << this->programExpression.toStringFull();
    this->outputJS[WebAPI::result::resultLabel]["input"] = startingExpression.toStringFull();
    this->outputJS[WebAPI::result::resultLabel]["output"] = this->programExpression.toStringFull();
  }
  this->outputString = out.str();
  this->outputJS[WebAPI::result::resultHtml] = out.str();
  std::stringstream commentsStream;
  if (this->objectContainer.algebraicClosure.latestBasis.size > 1) {
    commentsStream << "<b>Algebraic closure status.</b><br>"
    << this->objectContainer.algebraicClosure.toString();
  }
  if (this->objectContainer.constraints.size > 0) {
    commentsStream << "<b>Constraints.</b><br>";
    for (int i = 0; i < this->objectContainer.constraints.size; i ++) {
      commentsStream
      << HtmlRoutines::getMathNoDisplay(this->objectContainer.constraints[i].toString())
      << "<br>";
    }
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
