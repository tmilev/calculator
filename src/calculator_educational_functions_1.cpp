// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_educational_functions_1.h"
#include "calculator_inner_functions.h"
#include "string_constants.h"
#include "macros.h"

bool UnivariateEquation::getSolutions(Calculator& calculator, Expression& output) {
  if (this->solutions.size == 1) {
    output = this->solutions[0];
    return true;
  }
  return output.makeSequence(calculator, &this->solutions);
}

bool UnivariateEquation::getOneSolutionEquation(
  int solutionIndex, Calculator& calculator, Expression& output
) {
  return output.makeXOX(
    calculator,
    calculator.opDefine(),
    this->variable,
    this->solutions[solutionIndex]
  );
}

bool UnivariateEquation::getSolutionEquations(Calculator& calculator, Expression& output) {
  if (this->solutions.size == 1) {
    return this->getOneSolutionEquation(0, calculator, output);
  }
  output.makeSequence(calculator);
  for (int i = 0; i < this->solutions.size; i ++) {
    Expression nextSolution;
    this->getOneSolutionEquation(i, calculator, nextSolution);
    output.addChildOnTop(nextSolution);
  }
  return true;
}

bool UnivariateEquation::solve(Calculator& calculator){
  if (!Calculator::evaluateExpression(
    calculator, this->equationAllTermsOnLeftHandSide, this->simplified
  )) {
    return calculator << "Failed to simplify: "
    << this->equationAllTermsOnLeftHandSide.toString();
  }
  Expression powers;
  Expression powerExtractor;
  powerExtractor.makeXOX(
    calculator,
    calculator.opCoefficientsPowersOf(),
    this->variable,
    this->simplified
  );
  if (!CalculatorFunctions::coefficientsPowersOf(
    calculator, powerExtractor, powers
  )) {
    std::string simplifiedString = this->simplified.toString();
    calculator << "Failed to extract the coefficients of "
    << this->variable.toString()
    << " in " << simplifiedString << ". ";
    std::string originalString = this->equationAllTermsOnLeftHandSide.toString();
    if (originalString != simplifiedString) {
      calculator << "This was obtained from the original epxression "
      << this->equationAllTermsOnLeftHandSide.toString();
    }
    return false;
  }
  if (!powers.isSequenceNElements()) {
    return calculator << "This is not supposed to happen: expression "
    << powers.toString()
    << " should be a list. This may be a programming bug. ";
  }
  if (powers.size() == 2) {
    return calculator << "Cannot solve: " << this->equationAllTermsOnLeftHandSide.toString()
    << ". The expression does not depend on " << this->variable.toString()
    << ". The coefficients of "
    << this->variable.toString() << " are: " << powers.toString();
  }
  if (powers.size() == 3) {
    Expression solution;
    solution = powers[1];
    solution *= - 1;
    solution /= powers[2];
    this->solutions.addOnTop(solution);
    return true;
  }
  if (powers.size() == 4) {
    const Expression& a = powers[3];
    const Expression& b = powers[2];
    const Expression& c = powers[1];
    Expression currentRoot;
    Expression discriminant;
    discriminant = b * b - a * c * 4;
    Expression sqrtDiscriminant;
    sqrtDiscriminant.makeSqrt(calculator, discriminant, 2);
    currentRoot = (b * (- 1) - sqrtDiscriminant) / (a * 2);
    this->solutions.addOnTop(currentRoot);
    currentRoot = (b * (- 1) + sqrtDiscriminant) / (a * 2);
    this->solutions.addOnTop(currentRoot);
    return true;
  }
  Polynomial<Rational> polynomial;
  for (int i = powers.size() - 1; i >= 1; i --) {
    MonomialPolynomial oneVariable(0, 1);
    polynomial *= oneVariable;
    Rational coefficient;
    if (!powers[i].isRational(&coefficient)) {
      return false;
    }
    polynomial += coefficient;
  }
  List<AlgebraicNumber> solutions;
  if (PolynomialFactorizationKronecker::solvePolynomial(
    polynomial,
    solutions,
    calculator.objectContainer.algebraicClosure,
    &calculator.comments
  )) {
    for (int i = 0; i < solutions.size; i ++) {
      Expression currentSolution;
      currentSolution.assignValue(calculator, solutions[i]);
      this->solutions.addOnTop(currentSolution);
    }
    return true;
  }
  return false;
}

bool CalculatorFunctions::solveUnivariatePolynomialWithRadicalsWithRespectTo(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::solveUnivariatePolynomialWithRadicalsWithRespectTo");
  if (input.size() != 3) {
    return calculator << "SolveFor takes as input three arguments. ";
  }
  if (input[1].hasBoundVariables()) {
    return false;
  }
  if (input[2].hasBoundVariables()) {
    return false;
  }
  Expression modifiedInput = input;
  UnivariateEquation equation;
  equation.variable = modifiedInput[1];
  if (!modifiedInput[2].startsWith(calculator.opDefine())) {
    equation.equationAllTermsOnLeftHandSide = modifiedInput[2];
  } else if (!CalculatorFunctions::functionEqualityToArithmeticExpression(calculator, modifiedInput[2], equation.equationAllTermsOnLeftHandSide)) {
    return calculator << "Failed to interpret the equality " << modifiedInput[2].toString();
  }
  if (!equation.solve(calculator)) {
    return false;
  }
  return equation.getSolutions(calculator, output);
}

bool ProblemWithSolution::solve(Calculator& calculator) {
  if (this->solveEquation(calculator)) {
    return true;
  }
  return this->solveOther(calculator);
}

bool ProblemWithSolution::solveEquation(Calculator& calculator) {
  MacroRegisterFunctionWithName("ProblemWithSolution::solveEquation");
  if (!this->toBeSolved.startsWith(calculator.opDefine(), 3)) {
    return false;
  }
  Expression leftMinusRight;
  if (!CalculatorFunctions::functionEqualityToArithmeticExpression(
    calculator, this->toBeSolved, leftMinusRight
  )) {
    return false;
  }
  HashedList<Expression> freeVariables;
  leftMinusRight.getFreeVariables(freeVariables, false);
  if (freeVariables.size == 1) {
    UnivariateEquation equation;
    equation.variable = freeVariables.getElement(0);
    equation.equationAllTermsOnLeftHandSide = leftMinusRight;
    if (!equation.solve(calculator)) {
      this->addAnnotationStep(
        calculator,
        "Sorry, I haven't been taught how to solve this equation."
      );
      return true;
    }
    equation.getSolutionEquations(calculator, this->solutionsNonSimplified);
    this->solutionsSimplified = this->solutionsNonSimplified;
    calculator.evaluateExpression(calculator, this->solutionsNonSimplified, this->solutionsSimplified);
    if (this->solutionsNonSimplified != this->solutionsSimplified) {
      Calculator::ExpressionHistoryEnumerator::Step step;
      step.assignContentAndAnnotation(this->solutionsNonSimplified, "non-simplified solutions");
      this->steps.addOnTop(step);
    }
    Calculator::ExpressionHistoryEnumerator::Step finalAnswer;
    finalAnswer.assignContentAndAnnotation(this->solutionsSimplified, "final answer");
    this->steps.addOnTop(finalAnswer);
    return true;
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (!CalculatorConversions::convertToPolynomial(leftMinusRight, polynomial)) {
    this->addAnnotationStep(calculator, "Sorry, I've only been taught to solve polynomial equations.");
    return true;
  }
  if (polynomial.content.minimalNumberOfVariables() == 2) {
    this->addAnnotationStep(calculator, "This is an equation in two variables. Such equations are plots of curves. ");
    return true;
  }
  return false;
}

void ProblemWithSolution::addAnnotationStep(Calculator& calculator, const std::string& input) {
  Calculator::ExpressionHistoryEnumerator::Step step;
  step.content.assignValueOLD(input, calculator);
  this->steps.addOnTop(step);
}

bool ProblemWithSolution::solveOther(Calculator& calculator) {
  MacroRegisterFunctionWithName("ProblemWithSolution::solveOther");
  bool outputNonCacheable = false;
  Calculator::ExpressionHistoryEnumerator history;
  calculator.evaluateExpression(
    calculator,
    this->toBeSolved,
    this->solutionsSimplified,
    outputNonCacheable,
    - 1,
    &history.history
  );
  history.owner = &calculator;
  std::stringstream commentsOnFailure;
  if (!history.computeRecursively(0, &commentsOnFailure)) {
    this->error = commentsOnFailure.str();
  }
  history.toSteps(this->steps);
  return false;
}

bool CalculatorEducationalFunctions::solveJSON(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorEducationalFunctions::solveJSON");
  if (input.size() != 2) {
    return false;
  }
  ProblemWithSolution problem;
  problem.toBeSolved = input[1];
  problem.solve(calculator);
  return output.assignValue(calculator, problem.toJSON());
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
    return output.assignValueOLD(comparison.toJSON(), calculator);
  }
  if (!comparison.given.getFreeVariables(comparison.freeVariablesFound, false)) {
    comparison.errorEvaluation = "Unexpected failure to extract free variables from given answer.";
    return output.assignValueOLD(comparison.toJSON(), calculator);
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
    return output.assignValueOLD(comparison.toJSON(), calculator);
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
  return output.assignValueOLD(comparison.toJSON(), calculator);
}

void CompareExpressions::processComparisonRestricted() {
  MacroRegisterFunctionWithName("CompareExpressions::processComparisonRestricted");
  this->flagAreEqualAsAnswers = this->comparisonNoDistributionEvaluated.toString() == "1";
  this->flagAreEqual = this->comparisonStandardEvaluated.toString() == "1";
}

JSData Calculator::extractSolution() {
  MacroRegisterFunctionWithName("Calculator::extractSolution");
  JSData result;
  if (this->parser.syntaxErrors != "") {
    result[WebAPI::result::error] = "Failed to parse.";
    result[WebAPI::result::syntaxErrors] = this->parser.toStringSyntacticStackHTMLSimple();
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
  if (!calculator.parser.parseNoEmbeddingInCommand(this->givenString, this->given)) {
    this->syntaxErrorsLeftRaw = "Error parsing given expression." + calculator.parser.syntaxErrors;
    this->syntaxErrorsLeftFormatted = calculator.parser.toStringSyntacticStackHTMLSimple();
    return;
  }
  if (!calculator.parser.parseNoEmbeddingInCommand(this->desiredString, this->desired)) {
    this->syntaxErrorsRightRaw =  "Error parsing desired expression." + calculator.parser.syntaxErrors;
    this->syntaxErrorsRightFormatted = calculator.parser.toStringSyntacticStackHTMLSimple();
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
  JSData stepsJSON = JSData::makeEmptyArray();
  for (int i = 0; i < this->steps.size; i ++){
    stepsJSON[i] = this->steps[i].toJSON();
  }
  result[WebAPI::result::SolutionData::steps] = stepsJSON;
  result[WebAPI::result::SolutionData::finalExpression] = this->solutionsSimplified.toString();
  result[WebAPI::result::SolutionData::comments] = this->comments;
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
    return output.assignError(calculator, "Division by zero. ");
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
    return output.assignValueOLD(result, calculator);
  }
  return false;
}
