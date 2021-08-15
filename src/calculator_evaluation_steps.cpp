// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_interface.h"
#include "string_constants.h"

void Calculator::ExpressionHistoryEnumerator::initializeComputation() {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::initializeComputation");
  this->output.setSize(0);
  this->rulesNames.setSize(0);
}

Calculator::ExpressionHistoryEnumerator::ExpressionHistoryEnumerator() {
  this->owner = nullptr;
  this->maximumRecursionDepth = 1000;
  this->recursionDepth = 0;
}

bool Calculator::ExpressionHistoryEnumerator::computeRecursively(
  int incomingRecursionDepth, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::computeRecursively");
  this->recursionDepth = incomingRecursionDepth;
  if (recursionDepth > this->maximumRecursionDepth) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "While computing expression history, "
      << "exceeded maximum recursion depth of "
      << this->maximumRecursionDepth << ". ";
    }
    return false;
  }
  if (!this->history.startsWith(this->owner->opExpressionHistory())) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Corrupt expression history does not start with the expected atom: "
      << this->history.toString();
      return false;
    }
  }
  int childrenToAccount = 0;
  int firstNonAccountedChildIndex = - 1;
  for (int i = 1; i < this->history.size(); i ++) {
    const Expression& current = this->history[i];
    if (current.startsWith(this->owner->opExpressionHistorySetChild())) {
      if (childrenToAccount == 0) {
        firstNonAccountedChildIndex = i;
      }
      childrenToAccount ++;
      continue;
    }
    if (!this->processChildrenTransformations(
      firstNonAccountedChildIndex,
      childrenToAccount,
      commentsOnFailure
    )) {
      return false;
    }
    childrenToAccount = 0;
    if (!this->processTransformation(current, commentsOnFailure)) {
      return false;
    }
  }
  if (!this->processChildrenTransformations(
    firstNonAccountedChildIndex, childrenToAccount, commentsOnFailure
  )) {
    return false;
  }
  return true;
}

bool Calculator::ExpressionHistoryEnumerator::processChildrenTransformations(
  int startIndex, int numberOfChildren, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::processChildrenTransformations");
  if (numberOfChildren <= 0) {
    return true;
  }
  List<int> indicesInParent;
  List<ExpressionHistoryEnumerator> childrenEnumerators;
  childrenEnumerators.setSize(numberOfChildren);
  indicesInParent.setSize(numberOfChildren);
  for (int i = 0; i < numberOfChildren; i ++) {
    const Expression& current = this->history[startIndex + i];
    if (current.size() < 3) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "ExpressionHistorySetChild "
        << "expression does not have enough children. "
        << current.toString() ;
      }
      return false;
    }
    if (!current[1].isSmallInteger(&indicesInParent[i])) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "ExpressionHistorySetChild: bad child index. "
        << current.toString() ;
      }
      return false;
    }
    childrenEnumerators[i].owner = this->owner;
    childrenEnumerators[i].history = current[2];
    if (!childrenEnumerators[i].computeRecursively(
      this->recursionDepth + 1, commentsOnFailure
    )) {
      return false;
    }
  }
  if (this->output.size == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Missing starting expression. History: " << this->history.toString();
    }
    return false;
  }
  List<int> currentIndices;
  currentIndices.initializeFillInObject(numberOfChildren, 0);
  while (true) {
    bool found = false;
    Expression next;
    next = *this->output.lastObject();
    List<std::string> nextRules;
    for (int i = 0; i < numberOfChildren; i ++) {
      int& currentIndex = currentIndices[i];
      List<Expression>& expressionSequence = childrenEnumerators[i].output;
      if (currentIndex + 1 < expressionSequence.size) {
        currentIndex ++;
        found = true;
        int indexInParent = indicesInParent[i];
        next.setChild(indexInParent, expressionSequence[currentIndex]);
        nextRules.addListOnTop(childrenEnumerators[i].rulesNames[currentIndex]);
      }
    }
    if (!found) {
      break;
    }
    this->output.addOnTop(next);
    this->rulesNames.addOnTop(nextRules);
  }
  return true;
}

bool Calculator::ExpressionHistoryEnumerator::processTransformation(
  const Expression& current, std::stringstream* commentsOnFailure
) {
  if (!current.startsWith(this->owner->opExpressionHistorySet())) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad first atom in history child: " << current.toString();
    }
    return false;
  }
  if (current.size() < 2) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Not enough elements in history child: " << current.toString();
    }
    return false;
  }
  this->output.addOnTop(current[1]);
  this->rulesNames.setSize(this->rulesNames.size + 1);
  this->rulesNames.lastObject()->setSize(0);
  if (current.size() >= 3) {
    std::string incoming = current[2].toString();
    if (incoming != "") {
      this->rulesNames.lastObject()->addOnTop(incoming);
    }
  }
  return true;
}

std::string Calculator::ExpressionHistoryEnumerator::toStringDebug() {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::toStringDebug");
  std::stringstream out;
  out << "<b>History</b><br>" << this->history.toStringTreeHtml(- 1) << "<hr>";
  out << "" << this->history.toStringSemiFull() << "<hr>";
  return out.str();
}

void Calculator::ExpressionHistoryEnumerator::toStepsNoMerging(
  List<Calculator::ExpressionHistoryEnumerator::Step>& outputSteps
) {
  outputSteps.setSize(0);
  for (int j = 0; j < this->output.size; j ++) {
    Calculator::ExpressionHistoryEnumerator::Step currentStep;
    if (j > 0) {
      currentStep.stepType = "=";
    }
    this->toOneStep(j, currentStep);
    outputSteps.addOnTop(currentStep);
  }
}

void Calculator::ExpressionHistoryEnumerator::toSteps(
  List<Calculator::ExpressionHistoryEnumerator::Step>& outputSteps
) {
  List<Calculator::ExpressionHistoryEnumerator::Step> stepsUnmerged;
  List<Calculator::ExpressionHistoryEnumerator::Step> stepsMerged;
  this->toStepsNoMerging(stepsUnmerged);
  this->toStepsWithMerge(stepsUnmerged, stepsMerged);
  this->toStepsCleanUp(stepsMerged, outputSteps);
}

void Calculator::ExpressionHistoryEnumerator::toStepsWithMerge(
  List<Calculator::ExpressionHistoryEnumerator::Step>& unmerged,
  List<Calculator::ExpressionHistoryEnumerator::Step>& outputSteps
) {
  outputSteps.setSize(0);
  std::string previousExpressionString;
  for (int j = 0; j < unmerged.size; j ++) {
    Calculator::ExpressionHistoryEnumerator::Step& current = unmerged[j];
    std::string currentString = current.content.toString();
    if (currentString == previousExpressionString) {
      if (outputSteps.size > 0) {
        outputSteps.lastObject()->mergeAnnotations(current.annotations);
      }
      continue;
    }
    previousExpressionString = currentString;
    outputSteps.addOnTop(current);
  }
}

List<std::string> Calculator::ExpressionHistoryEnumerator::ruleIgnoreList({
  "IntegralOperatorFromProduct",
  "InterpretAsDifferential",
  "IntegralOperatorFromProduct",
  "CommuteConstants",
  "MultiplyByOne"
});
HashedList<std::string, MathRoutines::hashString> Calculator::ExpressionHistoryEnumerator::ruleIgnoreListHashList;

void Calculator::ExpressionHistoryEnumerator::toStepsCleanUp(
  List<Calculator::ExpressionHistoryEnumerator::Step>& raw,
  List<Calculator::ExpressionHistoryEnumerator::Step>& outputSteps
) {
  outputSteps.setSize(0);
  for (int i = 0; i < raw.size; i ++) {
    if (this->isIgnorable(raw[i])) {
      continue;
    }
    outputSteps.addOnTop(raw[i]);
  }
}

bool Calculator::ExpressionHistoryEnumerator::isIgnorable(
  const Calculator::ExpressionHistoryEnumerator::Step& step
) {
  List<std::string>& ignoreList = Calculator::ExpressionHistoryEnumerator::ruleIgnoreList;
  HashedList<std::string, MathRoutines::hashString>& ignoreSet =
  Calculator::ExpressionHistoryEnumerator::ruleIgnoreListHashList;
  if (ignoreList.size > 0 && ignoreSet.size == 0) {
    ignoreSet.addListOnTop(ignoreList);
  }
  for (int i = 0; i < step.annotations.size; i ++) {
    if (!ignoreSet.contains(step.annotations[i])) {
      return false;
    }
  }
  return true;
}

void Calculator::ExpressionHistoryEnumerator::Step::mergeAnnotations(
  const List<std::string>& incoming
) {
  this->annotations.addListOnTop(incoming);
}

JSData Calculator::ExpressionHistoryEnumerator::Step::toJSON() {
  JSData result;
  result[WebAPI::result::SolutionData::transformation] = this->content.toString();
  JSData annotationJSON = JSData::makeEmptyArray();
  for (int i = 0; i < this->annotations.size; i ++) {
    annotationJSON[i] = this->annotations[i];
  }
  result[WebAPI::result::SolutionData::stepType] = this->stepType;
  result[WebAPI::result::SolutionData::annotations] = annotationJSON;
  return result;
}

void Calculator::ExpressionHistoryEnumerator::Step::assignContentAndAnnotation(
  const Expression& input, const std::string& oneAnnotation
) {
  this->content = input;
  this->annotations.setSize(0);
  this->annotations.addOnTop(oneAnnotation);
}

void Calculator::ExpressionHistoryEnumerator::toOneStep(
  int stepIndex,
  Calculator::ExpressionHistoryEnumerator::Step& outputStep
) {
  outputStep.annotations.addListOnTop(this->rulesNames[stepIndex]);
  outputStep.content = this->output[stepIndex];
}

std::string Calculator::ExpressionHistoryEnumerator::toStringExpressionHistoryMerged() {
  std::stringstream out;
  out << "\\(\\begin{array}{ll|l}";
  std::string prevEstring = "";
  List<std::string> currentRules;
  for (int j = 0; j < this->output.size; j ++) {
    std::string currentEstring = this->output[j].toString();
    if (currentEstring == prevEstring) {
      currentRules.addListOnTop(this->rulesNames[j]);
      continue;
    }
    prevEstring = currentEstring;
    currentRules.addListOnTop(this->rulesNames[j]);
    if (j > 0) {
      if (currentRules.size > 0) {
        out << "&" ;
        for (int i = 0; i < currentRules.size; i ++) {
          out << currentRules[i];
          if (i != currentRules.size - 1) {
            out << ",~";
          }
        }
      }
      currentRules.setSize(0);
      out << "\\\\";
      out << "=";
    }
    out << "&\n" << currentEstring;
  }
  if (currentRules.size > 0) {
    out << "&\\text{apply: " << currentRules.toStringCommaDelimited() << "}";
  }
  out << "\\end{array}\\)";
  return out.str();
}

bool CalculatorSteps::innerLogEvaluationStepsDebug(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return CalculatorSteps::innerLogEvaluationStepsHumanReadableMerged(calculator, input, output, true);
}

bool CalculatorSteps::innerLogEvaluationSteps(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return CalculatorSteps::innerLogEvaluationStepsHumanReadableMerged(calculator, input, output, false);
}

bool CalculatorSteps::innerLogEvaluationStepsHumanReadableMerged(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool doDebug
) {
  MacroRegisterFunctionWithName("CalculatorSteps::innerLogEvaluationStepsHumanReadableMerged");
  Expression argument;
  if (input.size() == 2) {
    argument = input[1];
  } else {
    argument = input;
    argument.setChildAtomValue(0, calculator.opSequence());
  }
  Expression outputTransformation;
  bool notUsed = false;
  Calculator::ExpressionHistoryEnumerator history;
  calculator.evaluateExpression(
    calculator,
    argument,
    outputTransformation,
    notUsed,
    - 1,
    &history.history
  );
  std::stringstream out;
  history.owner = &calculator;
  history.computeRecursively(0, &out);
  out << history.toStringExpressionHistoryMerged();
  if (doDebug) {
    out << "<hr>" << history.toStringDebug();
  }
  return output.assignValueOLD(out.str(), calculator);
}
