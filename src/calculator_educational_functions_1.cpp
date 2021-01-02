// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_educational_functions_1.h"
#include "string_constants.h"
#include "macros.h"

bool CalculatorEducationalFunctions::solveJSON(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorEducationalFunctions::solveJSON");
  if (input.size() != 2) {
    return false;
  }
  ProblemWithSolution problem;
  problem.toBeSolved = input[1];
  bool outputNonCacheable = false;
  Calculator::ExpressionHistoryEnumerator history;
  calculator.evaluateExpression(
    calculator,
    problem.toBeSolved,
    problem.finalExpression,
    outputNonCacheable,
    - 1,
    &history.theHistory
  );
  history.owner = &calculator;
  std::stringstream commentsOnFailure;
  if (!history.computeRecursively(0, &commentsOnFailure)) {
    problem.error = commentsOnFailure.str();
  }
  problem.solution = history.toStringExpressionHistoryMerged();
  return output.assignValue(problem.toJSON(), calculator);
}

bool CalculatorEducationalFunctions::compareExpressionsJSON(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorEducationalFunctions::compareExpressionsJSON");
  if (input.size() != 3) {
    return false;
  }
  CompareExpressions comparison;
  comparison.given = input[1];
  comparison.desired = input[2];
  comparison.comparisonRaw.makeXOX(
    calculator, calculator.opEqualEqual(), comparison.given, comparison.desired
  );

  calculator.evaluateExpression(
    calculator,
    comparison.comparisonRaw,
    comparison.comparisonEvaluated
  );
  JSData result;
  result["comparison"] = comparison.comparisonEvaluated.toString();
  return output.assignValue(result, calculator);
}

JSData Calculator::extractSolution() {
  MacroRegisterFunctionWithName("Calculator::extractSolution");
  JSData result;
  if (this->syntaxErrors != "") {
    result[WebAPI::result::error] = this->toStringSyntacticStackHumanReadable(false, true);
    return result;
  }
  JSData solutionJSON;
  if (!this->programExpression.isOfType(&solutionJSON)) {
    result[WebAPI::result::error] = "Could not solve your problem. ";
    return result;
  } else {
    result[WebAPI::result::solution] = solutionJSON;
  }
  return result;
}

JSData Calculator::extractComparison() {
  MacroRegisterFunctionWithName("Calculator::extractComparison");
  JSData result;
  if (this->syntaxErrors != "") {
    result[WebAPI::result::error] = this->toStringSyntacticStackHumanReadable(false, true);
    return result;
  }
  JSData comparisonJSON;
  if (!this->programExpression.isOfType(&comparisonJSON)) {
    result[WebAPI::result::error] = "Could not solve your problem. ";
    return result;
  } else {
    result[WebAPI::result::solution] = comparisonJSON;
  }
  return result;
}

JSData ProblemWithSolution::toJSON() {
  JSData result;
  result[WebAPI::result::solutionData::input] = this->toBeSolved.toString();
  result[WebAPI::result::solutionData::steps] = this->solution;
  result[WebAPI::result::solutionData::finalExpression] = this->finalExpression.toString();
  result[WebAPI::result::error] = this->error;
  return result;
}
