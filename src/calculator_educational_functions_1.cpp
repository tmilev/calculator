// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_educational_functions_1.h"
#include "string_constants.h"
#include "macros.h"

bool CalculatorEducationalFunctions::solveJSON(
  Calculator& calculator, const Expression& input, Expression& output
) {
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
  Calculator& calculator,
  const Expression& input,
  Expression& output
) {
  CompareExpressions comparison(false);
  return CalculatorEducationalFunctions::compareExpressionsJSONInternal(
    calculator, input, output, comparison
  );
}

bool CalculatorEducationalFunctions::compareExpressionsJSONInternal(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  CompareExpressions& comparison
) {
  MacroRegisterFunctionWithName("CalculatorEducationalFunctions::compareExpressionsJSONInternal");
  if (input.size() != 3) {
    return false;
  }
  if (comparison.givenString == "") {
    comparison.givenString = comparison.given.toString();
  }
  if (comparison.desiredString == "") {
    comparison.desiredString = comparison.desired.toString();
  }
  comparison.given = input[1];
  comparison.desired = input[2];
  if (!comparison.desired.getFreeVariables(comparison.freeVariablesDesired, false)) {
    comparison.errorEvaluation = "Unexpected failure to extract free variables from desired answer.";
    return output.assignValue(comparison.toJSON(), calculator);
  }
  if (!comparison.given.getFreeVariables(comparison.freeVariablesFound, false)) {
    comparison.errorEvaluation = "Unexpected failure to extract free variables from given answer.";
    return output.assignValue(comparison.toJSON(), calculator);
  }
  for (int i = 0; i < comparison.freeVariablesFound.size; i ++) {
    const Expression& current = comparison.freeVariablesFound[i];
    if (!comparison.freeVariablesDesired.contains(current)) {
      comparison.unexpectedVariables.addOnTop(current);
    }
  }
  if (comparison.unexpectedVariables.size > 0) {
    std::stringstream errorStream;
    errorStream << "Unexpected symbols: <b style='color:red'>"
    << comparison.unexpectedVariables.toStringCommaDelimited() << "</b>";
    comparison.errorInAnswer = errorStream.str();
    return output.assignValue(comparison.toJSON(), calculator);
  }
  comparison.givenSimplified.makeOX(
    calculator,
    calculator.getOperations().getIndexNoFail("Polynomialize"),
    comparison.given
  );
  comparison.desiredSimplified.makeOX(
    calculator,
    calculator.getOperations().getIndexNoFail("Polynomialize"),
    comparison.desired
  );

  comparison.comparisonStandardRaw.makeXOX(
    calculator,
    calculator.opEqualEqual(),
    comparison.givenSimplified,
    comparison.desiredSimplified
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
    "(TurnOffRules("
    "DistributeMultiplication, "
    "AddRationals, "
    "AddTerms, "
    "DivideByNumber, "
    "ConvertShortDenominatorToNegativePower,"
    "DivideRationalByRational,"
    "ConstantExtraction"
    ");\n"
    "TurnOnRules(SortTerms, DivideByNumberTrivial);\n"
    "a===b)_3",
    &substitution,
    calculator
  );
  calculator.evaluateExpression(
    calculator,
    comparison.comparisonNoDistributionRaw,
    comparison.comparisonNoDistributionEvaluated
  );
  comparison.processComparisonRestricted();
  return output.assignValue(comparison.toJSON(), calculator);
}

void CompareExpressions::processComparisonRestricted() {
  MacroRegisterFunctionWithName("CompareExpressions::processComparisonRestricted");
  this->flagAreEqualAsAnswers = this->comparisonNoDistributionEvaluated.toString() == "1";
  this->flagAreEqual = this->comparisonStandardEvaluated.toString() == "1";
}

JSData Calculator::extractSolution() {
  MacroRegisterFunctionWithName("Calculator::extractSolution");
  JSData result;
  if (this->syntaxErrors != "") {
    result[WebAPI::result::error] = "Failed to parse.";
    result[WebAPI::result::syntaxErrors] = this->toStringSyntacticStackHTMLSimple();
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

void CompareExpressions::compare(
  const std::string& givenInput,
  const std::string& desiredInput,
  Calculator& calculator
) {
  MacroRegisterFunctionWithName("CompareExpressions::compare");
  this->givenString = givenInput;
  this->desiredString = desiredInput;
  calculator.inputString = "CompareExpressionsJSON{}(" + this->givenString + ", " + this->desiredString + ")";
  global.initOutputReportAndCrashFileNames(
    HtmlRoutines::convertStringToURLString(calculator.inputString, false),
    calculator.inputString
  );
  global.response.disallowReport();
  this->comparePartTwo(calculator);
  global.flagComputationComplete = true;
}

void CompareExpressions::comparePartTwo(Calculator& calculator) {
  MacroRegisterFunctionWithName("Calculator::compareExpressions");
  calculator.statistics.initialize();
  if (!calculator.parse(this->givenString, true, this->given)) {
    this->syntaxErrorsLeftRaw = "Error parsing given expression." + calculator.syntaxErrors;
    this->syntaxErrorsLeftFormatted = calculator.toStringSyntacticStackHTMLSimple();
    return;
  }
  if (!calculator.parse(this->desiredString, true, this->desired)) {
    this->syntaxErrorsRightRaw =  "Error parsing desired expression." + calculator.syntaxErrors;
    this->syntaxErrorsRightFormatted = calculator.toStringSyntacticStackHTMLSimple();
    return;
  }
  this->comparisonExpression.makeXOX(
    calculator,
    calculator.getOperations().getIndexNoFail("CompareExpressionsJSON"),
    this->given,
    this->desired
  );
  CalculatorEducationalFunctions::compareExpressionsJSONInternal(
    calculator,
    this->comparisonExpression,
    this->comparisonExpressionEvaluated,
    *this
  );
  global.flagComputationComplete = true;
}

CompareExpressions::CompareExpressions(bool hideDesiredAnswer) {
  this->flagAreEqual = false;
  this->flagAreEqualAsAnswers = false;
  this->flagHideDesiredAnswer = hideDesiredAnswer;
}

JSData CompareExpressions::toJSON() const {
  JSData result;
  result[WebAPI::result::commentsGlobal] = global.comments.getCurrentReset();
  result[WebAPI::result::ComparisonData::givenRaw] = this->givenString;
  if (!this->flagHideDesiredAnswer) {
    result[WebAPI::result::ComparisonData::desiredRaw] = this->desiredString;
  }
  if (this->syntaxErrorsLeftRaw != "") {
    result[WebAPI::result::error] = "Failed to parse the given (student) answer.";
    result[WebAPI::result::syntaxErrors] = this->syntaxErrorsLeftFormatted;
    result[WebAPI::result::syntaxErrorsExtra] = this->syntaxErrorsLeftRaw;
    return result;
  }
  if (this->syntaxErrorsRightRaw != "") {
    result[WebAPI::result::error] = "Failed to parse the desired (teacher) answer.";
    result[WebAPI::result::syntaxErrors] = this->syntaxErrorsRightFormatted;
    result[WebAPI::result::syntaxErrorsExtra] = this->syntaxErrorsRightRaw;
    return result;
  }
  if (this->errorEvaluation != "") {
    result[WebAPI::result::ComparisonData::errorEvaluation] = this->errorEvaluation;
  }
  if (this->errorInAnswer != "") {
    result[WebAPI::result::ComparisonData::errorInAnswer] = this->errorInAnswer;
  }
  result[WebAPI::result::ComparisonData::areEqual] = this->flagAreEqual;
  result[WebAPI::result::ComparisonData::areEqualAsAnswers] = this->flagAreEqualAsAnswers;
  if (
    this->syntaxErrorsLeftRaw != "" ||
    this->syntaxErrorsLeftFormatted != "" ||
    this->errorEvaluation != ""
  ) {
    return result;
  }
  std::stringstream resultHTML;
  if (!this->flagAreEqual) {
    resultHTML <<  "<b style='color:red;font-size: x-large;'>&cross;</b><br>";
  } else {
    if (!this->flagAreEqualAsAnswers) {
      resultHTML << "<b style='color:blue;font-size: x-large;'>&#x2713;</b> [more work needed]<br>";
    } else {
      resultHTML << "<b style='color:green;font-size: x-large;'>&#x2713;</b><br>";
    }
  }
  result[WebAPI::problem::answerWasHardCoded] = true;
  result[WebAPI::result::resultHtml] = resultHTML.str();
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

bool CalculatorEducationalFunctions::divideByNumberTrivial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorEducationalFunctions::divideByNumberTrivial");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (input[2].isEqualToZero()) {
    return output.makeError("Division by zero. ", calculator);
  }
  Rational numerator, denominator;
  if (!input[1].isRational(&numerator)) {
    return false;
  }
  if (!input[2].isRational(&denominator)) {
    return false;
  }
  Rational result = numerator / denominator;
  if (numerator == result.getNumerator() && denominator == result.getDenominator()) {
    return output.assignValue(result, calculator);
  }
  return false;
}
