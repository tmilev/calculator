// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_interface.h"

void Calculator::ExpressionHistoryEnumerator::initializeComputation() {
  MacroRegisterFunctionWithName("ExpressionHistoryEnumerator::initializeComputation");
  this->output.setSize(0);
  // this->rulesDisplayNames.setSize(0);
  this->rulesNames.setSize(0);
  // this->rulesToBeIgnored.clear();
  // this->rulesToBeIgnored.addOnTop("CommuteIfUnivariate");
  // this->rulesDisplayNamesMap.clear();
  // this->rulesDisplayNamesMap.setKeyValue("minus", "");
  // this->rulesDisplayNamesMap.setKeyValue("DistributeMultiplication", "");
  // this->rulesDisplayNamesMap.setKeyValue("MultiplyRationals", "");
  // this->rulesDisplayNamesMap.setKeyValue("ConstantExtraction", "");
  // this->rulesDisplayNamesMap.setKeyValue("MultiplyByOne", "");
  // this->rulesDisplayNamesMap.setKeyValue("AddTerms", "");
  // this->rulesDisplayNamesMap.setKeyValue("AssociativeRule", "");
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
  if (!this->theHistory.startsWith(this->owner->opExpressionHistory())) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Corrupt expression history does not start with the expected atom: "
      << this->theHistory.toString();
      return false;
    }
  }
  int childrenToAccount = 0;
  int firstNonAccountedChildIndex = - 1;
  for (int i = 1; i < this->theHistory.size(); i ++) {
    const Expression& current = this->theHistory[i];
    if (current.startsWith(this->owner->opExpressionHistorySetChild())) {
      if (childrenToAccount == 0) {
        firstNonAccountedChildIndex = i;
      }
      childrenToAccount ++;
      continue;
    }
    if (!this->processChildrenTransformations(firstNonAccountedChildIndex, childrenToAccount, commentsOnFailure)) {
      return false;
    }
    childrenToAccount = 0;
    if (!this->processTransformation(current, commentsOnFailure)) {
      return false;
    }
  }
  if (!this->processChildrenTransformations(firstNonAccountedChildIndex, childrenToAccount, commentsOnFailure)) {
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
    const Expression& current = this->theHistory[startIndex + i];
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
    childrenEnumerators[i].theHistory = current[2];
    if (!childrenEnumerators[i].computeRecursively(
      this->recursionDepth + 1, commentsOnFailure
    )) {
      return false;
    }
  }
  if (this->output.size == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Missing starting expression. History: " << this->theHistory.toString();
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
  out << "<b>History</b><br>" << this->theHistory.toStringTreeHtml(- 1) << "<hr>";
  out << "" << this->theHistory.toStringSemiFull() << "<hr>";
  // out << "Current state: " << this->currentState.toString()
  // << "<br>";
  // out << "Current subtree: " << this->currentSubTree.toString();

  return out.str();
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
    &history.theHistory
  );
  std::stringstream out;
  history.owner = &calculator;
  history.computeRecursively(0, &out);
  out << history.toStringExpressionHistoryMerged();
  if (doDebug) {
    out << "<hr>" << history.toStringDebug();
  }
  return output.assignValue(out.str(), calculator);
}
