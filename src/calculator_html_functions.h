#ifndef calculator_html_functions_H
#define calculator_html_functions_H

#include "calculator_interface.h"
static ProjectInformationInstance projectInfocClculatorHtmlFunctionsHeader(__FILE__, "Header, calculator html functions. ");

class Answer;

class CalculatorHtmlFunctions{
public:
  static bool innerExtractCalculatorExpressionFromHtml(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerInterpretProblemGiveUp(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerInterpretProblem(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerUserInputBox(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetInputBox(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerEvaluateSymbols(Calculator& theCommands, const Expression& input, Expression& output);
  static std::string GetUserInputBoxName(const Expression& theBox);
};

#endif // calculator_html_functions_H

