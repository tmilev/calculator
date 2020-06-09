// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "calculator_html_functions.h"

bool CalculatorHtmlFunctions::innerUserInputBox(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorHtmlFunctions::innerUserInputBox");
  MapList<std::string, Expression, MathRoutines::hashString> theArguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementList(calculator, input, theArguments, &calculator.comments)) {
    return false;
  }
  if (!theArguments.contains("name")) {
    return calculator << "User input name not specified in: " << input.toString();
  }
  std::string boxName = CalculatorHtmlFunctions::getUserInputBoxName(input);
  if (calculator.theObjectContainer.theUserInputTextBoxesWithValues.contains(boxName)) {
    return output.assignValue(calculator.theObjectContainer.theUserInputTextBoxesWithValues.getValueCreate(boxName), calculator);
  }
  InputBox newBox;
  newBox.name = boxName;
  for (int i = 0; i < theArguments.theKeys.size; i ++) {
    if (theArguments.theKeys[i] == "value") {
      newBox.value = theArguments.theValues[i];
    }
    if (theArguments.theKeys[i] == "min") {
      newBox.min = theArguments.theValues[i];
    }
    if (theArguments.theKeys[i] == "max") {
      newBox.max = theArguments.theValues[i];
    }
    if (theArguments.theKeys[i] == "step") {
      newBox.step = theArguments.theValues[i];
    }
  }
  return output.assignValue(newBox, calculator);
}

bool CalculatorHtmlFunctions::innerEvaluateSymbols(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorHtmlFunctions::innerEvaluateSymbols");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string theString;
  if (!argument.isOfType(&theString)) {
    return false;
  }
  List<SyntacticElement> theElts;
  calculator.parseFillDictionary(theString, theElts);
  Expression evaluatedE;
  std::stringstream out;
  bool previousWasInteger = false;
  for (int i = 0; i < theElts.size; i ++) {
    SyntacticElement& currentElt = theElts[i];
    if (currentElt.controlIndex == calculator.conVariable()) {
      calculator.evaluateExpression(calculator, currentElt.theData, evaluatedE);
      out << evaluatedE.toString();
      continue;
    }
    if (currentElt.controlIndex == calculator.conInteger()) {
      if (!previousWasInteger) {
        out << "{";
      }
      out << currentElt.theData.toString();
      previousWasInteger = true;
      continue;
    }
    if (previousWasInteger) {
      out << "}";
    }
    previousWasInteger = false;
    out << calculator.controlSequences[currentElt.controlIndex];
  }
  if (previousWasInteger) {
    out << "}";
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorHtmlFunctions::innerSetInputBox(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorHtmlFunctions::innerUserInputBox");
  MapList<std::string, Expression, MathRoutines::hashString> theArguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementList(
    calculator, input, theArguments, &calculator.comments
  )) {
    return false;
  }
  if (!theArguments.contains("name")) {
    return calculator << "User input name not specified in: " << input.toString();
  }
  if (!theArguments.contains("value")) {
    return calculator << "Input box value not specified in: " << input.toString();
  }
  std::string boxName = CalculatorHtmlFunctions::getUserInputBoxName(input);
  if (calculator.theObjectContainer.theUserInputTextBoxesWithValues.contains(boxName)) {
    return calculator << "Input box with name: " << boxName << " already has value.";
  }
  InputBox& theBox =
  calculator.theObjectContainer.theUserInputTextBoxesWithValues.getValueCreate(boxName);
  theBox.name = boxName;
  theBox.value = theArguments.getValueCreate("value");
  std::stringstream out;
  out << "Set value to input box name: " << boxName;
  return output.assignValue(out.str(), calculator);
}

std::string CalculatorHtmlFunctions::getUserInputBoxName(const Expression& theBox) {
  MacroRegisterFunctionWithName("CalculatorHtmlFunctions::getUserInputBoxName");
  if (theBox.owner == nullptr) {
    return "non-initialized-expression";
  }
  Calculator& calculator = *theBox.owner;
  MapList<std::string, Expression, MathRoutines::hashString> theArguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementList(calculator, theBox, theArguments)) {
    return "corrupt-box";
  }
  if (!theArguments.contains("name")) {
    return "box-without-name";
  }
  std::string theBoxName = "faultyBoxName";
  if (!theArguments.getValueCreate("name").isOfType<std::string>(&theBoxName)) {
    theBoxName = theArguments.getValueCreate("name").toString();
  }
  return theBoxName;
}
