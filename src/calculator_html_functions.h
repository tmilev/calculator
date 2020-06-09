// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef calculator_html_functions_H
#define calculator_html_functions_H

#include "calculator_interface.h"

class Answer;

class CalculatorHtmlFunctions{
public:
  static bool innerExtractCalculatorExpressionFromHtml(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerInterpretProblemGiveUp(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerInterpretProblem(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerUserInputBox(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSetInputBox(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerEvaluateSymbols(Calculator& calculator, const Expression& input, Expression& output);
  static std::string getUserInputBoxName(const Expression& theBox);
};

#endif // calculator_html_functions_H

