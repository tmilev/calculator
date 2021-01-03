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
  comparison.comparisonStandardRaw.makeXOX(
    calculator, calculator.opEqualEqual(), comparison.given, comparison.desired
  );
  calculator.evaluateExpression(
    calculator,
    comparison.comparisonStandardRaw,
    comparison.comparisonStandardEvaluated
  );
  MapList<std::string, Expression, MathRoutines::hashString> substitution;
  substitution.setKeyValue("a", comparison.given);
  substitution.setKeyValue("b", comparison.desired);
  comparison.comparisonNoDistributionRaw.assignStringParsed(
    "TurnOffRules(DistributeMultiplication);\n"
    "a==b",
    &substitution,
    calculator
  );
  calculator.evaluateExpression(
    calculator,
    comparison.comparisonNoDistributionRaw,
    comparison.comparisonNoDistributionEvaluated
  );

  JSData result;
  result[WebAPI::result::ComparisonData::given] = comparison.given.toString();
  result[WebAPI::result::ComparisonData::desired] = comparison.desired.toString();
  if (comparison.comparisonStandardEvaluated.toString() == "1") {
    result[WebAPI::result::ComparisonData::areEqual] = true;
  } else {
    result[WebAPI::result::ComparisonData::areEqual] = false;
  }
  if (comparison.comparisonNoDistributionEvaluated.toString() == "1") {
    result[WebAPI::result::ComparisonData::areEqualBarDistribution] = true;
  } else {
    result[WebAPI::result::ComparisonData::areEqualBarDistribution] = false;
  }
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

JSData Calculator::extractComparison(const std::string& given, const std::string& desired) {
  MacroRegisterFunctionWithName("Calculator::extractComparison");
  JSData result;
  result[WebAPI::result::ComparisonData::givenRaw] = given;
  result[WebAPI::result::ComparisonData::desiredRaw] = desired;
  if (this->syntaxErrors != "") {
    result[WebAPI::result::error] = this->toStringSyntacticStackHumanReadable(false, true);
    return result;
  }
  JSData comparisonJSON;
  if (!this->programExpression.isOfType(&comparisonJSON)) {
    result[WebAPI::result::error] = "Could not solve your problem. ";
    return result;
  } else {
    result[WebAPI::result::comparison] = comparisonJSON;
  }
  return result;
}

JSData ProblemWithSolution::toJSON() {
  JSData result;
  result[WebAPI::result::SolutionData::input] = this->toBeSolved.toString();
  result[WebAPI::result::SolutionData::steps] = this->solution;
  result[WebAPI::result::SolutionData::finalExpression] = this->finalExpression.toString();
  result[WebAPI::result::error] = this->error;
  return result;
}
