// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef calculator_html_functions_H
#define calculator_html_functions_H

#include "calculator_interface.h"

class Answer;

class CalculatorHtmlFunctions{
public:
  static bool extractCalculatorExpressionFromHtml(Calculator& calculator, const Expression& input, Expression& output);
  static bool interpretProblemGiveUp(Calculator& calculator, const Expression& input, Expression& output);
  static bool interpretProblem(Calculator& calculator, const Expression& input, Expression& output);
  static bool userInputBox(Calculator& calculator, const Expression& input, Expression& output);
  static bool setInputBox(Calculator& calculator, const Expression& input, Expression& output);
  static bool evaluateSymbols(Calculator& calculator, const Expression& input, Expression& output);
  static std::string getUserInputBoxName(const Expression& box);
};

#endif // calculator_html_functions_H

