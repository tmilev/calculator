#ifndef VPFHEADER3CALCULATOR4HTMLFUNCTIONS_H
#define VPFHEADER3CALCULATOR4HTMLFUNCTIONS_H

#include "vpfHeader3Calculator0_Interface.h"
static ProjectInformationInstance ProjectInfoVPFHEADER3CALCULATOR4HTMLFUNCTIONS_H(__FILE__, "Header, calculator html functions. ");

class Answer;

class CalculatorHtmlFunctions{
public:
  static bool innerExtractCalculatorExpressionFromHtml
  (Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerInterpretProblemGiveUp
  (Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerInterpretProblem
  (Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerUserInputBox
  (Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetInputBox
  (Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerEvaluateSymbols
  (Calculator& theCommands, const Expression& input, Expression& output);

  static std::string GetUserInputBox
(const Expression& theBox)
  ;
  static std::string GetUserInputBoxName
(const Expression& theBox);
  static std::string GetJavascriptMathQuillBox(Answer& theAnswer);
};


#endif // VPFHEADER3CALCULATOR4HTMLFUNCTIONS_H

