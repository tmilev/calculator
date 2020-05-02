// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef calculator_html_functions_H
#define calculator_html_functions_H

#include "calculator_interface.h"

class Answer;

class CalculatorHtmlFunctions{
public:
  static bool innerExtractCalculatorExpressionFromHtml(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerInterpretProblemGiveUp(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerInterpretProblem(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerUserInputBox(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetInputBox(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerEvaluateSymbols(Calculator& theCommands, const Expression& input, Expression& output);
  static std::string getUserInputBoxName(const Expression& theBox);
};

#endif // calculator_html_functions_H

