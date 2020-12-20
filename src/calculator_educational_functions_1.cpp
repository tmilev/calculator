// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_educational_functions_1.h"
#include "string_constants.h"

bool CalculatorEducationalFunctions::solveJSON(Calculator& calculator, const Expression& input, Expression& output) {
  (void) input;
  return output.assignValue(std::string("not implemented yet"), calculator);
}


JSData Calculator::extractSolution() {
  JSData result;
  if (this->syntaxErrors != "") {
    result[WebAPI::result::error] = this->toStringSyntacticStackHumanReadable(false, true);
    return result;
  }
  if (!this->programExpression.startsWith(this->opCommandSequence())) {
    result[WebAPI::result::error] = "Failed to extract sequence of commands.";
    return result;
  }
  if (this->programExpression.size() != 1) {
    result[WebAPI::result::error] = "Unexpected number of commands.";
    return result;
  }
  std::string solutionJSON;
  if (!this->programExpression[0].isOfType(&solutionJSON)) {
    result[WebAPI::result::error] = "Could not solve your problem.";
    return result;
  }
  result[WebAPI::result::solution] = solutionJSON;
  return result;
}
