
#ifndef header_calculator_html_functions_ALREADY_INCLUDED
#define header_calculator_html_functions_ALREADY_INCLUDED

#include "calculator_interface.h"

class Answer;

class CalculatorHtmlFunctions {
public:
  static bool extractCalculatorExpressionFromHtml(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool interpretProblemGiveUp(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool interpretProblem(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool userInputBox(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool evaluateSymbols(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static std::string getUserInputBoxName(const Expression& box);
};

#endif // header_calculator_html_functions_ALREADY_INCLUDED
