// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "calculator_html_functions.h"

bool CalculatorHtmlFunctions::userInputBox(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorHtmlFunctions::userInputBox");
  MapList<std::string, Expression, MathRoutines::hashString> theArguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementList(calculator, input, theArguments, &calculator.comments)) {
    return false;
  }
  if (!theArguments.contains("name")) {
    return calculator << "User input name not specified in: " << input.toString();
  }
  std::string boxName = CalculatorHtmlFunctions::getUserInputBoxName(input);
  if (calculator.objectContainer.userInputTextBoxesWithValues.contains(boxName)) {
    return output.assignValue(calculator.objectContainer.userInputTextBoxesWithValues.getValueCreate(boxName), calculator);
  }
  InputBox newBox;
  newBox.name = boxName;
  for (int i = 0; i < theArguments.keys.size; i ++) {
    if (theArguments.keys[i] == "value") {
      newBox.value = theArguments.values[i];
    }
    if (theArguments.keys[i] == "min") {
      newBox.min = theArguments.values[i];
    }
    if (theArguments.keys[i] == "max") {
      newBox.max = theArguments.values[i];
    }
    if (theArguments.keys[i] == "step") {
      newBox.step = theArguments.values[i];
    }
  }
  return output.assignValue(newBox, calculator);
}

bool CalculatorHtmlFunctions::evaluateSymbols(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorHtmlFunctions::evaluateSymbols");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string theString;
  if (!argument.isOfType(&theString)) {
    return false;
  }
  List<SyntacticElement> elements;
  calculator.parser.parseFillDictionary(theString, elements);
  Expression evaluatedE;
  std::stringstream out;
  bool previousWasInteger = false;
  for (int i = 0; i < elements.size; i ++) {
    SyntacticElement& currentElt = elements[i];
    if (currentElt.controlIndex == calculator.parser.conVariable()) {
      calculator.evaluateExpression(calculator, currentElt.data, evaluatedE);
      out << evaluatedE.toString();
      continue;
    }
    if (currentElt.controlIndex == calculator.parser.conInteger()) {
      if (!previousWasInteger) {
        out << "{";
      }
      out << currentElt.data.toString();
      previousWasInteger = true;
      continue;
    }
    if (previousWasInteger) {
      out << "}";
    }
    previousWasInteger = false;
    out << calculator.parser.controlSequences[currentElt.controlIndex];
  }
  if (previousWasInteger) {
    out << "}";
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorHtmlFunctions::setInputBox(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorHtmlFunctions::setInputBox");
  MapList<std::string, Expression, MathRoutines::hashString> arguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementList(
    calculator, input, arguments, &calculator.comments
  )) {
    return false;
  }
  if (!arguments.contains("name")) {
    return calculator << "User input name not specified in: " << input.toString();
  }
  if (!arguments.contains("value")) {
    return calculator << "Input box value not specified in: " << input.toString();
  }
  std::string boxName = CalculatorHtmlFunctions::getUserInputBoxName(input);
  if (calculator.objectContainer.userInputTextBoxesWithValues.contains(boxName)) {
    return calculator << "Input box with name: " << boxName << " already has value.";
  }
  InputBox& box =
  calculator.objectContainer.userInputTextBoxesWithValues.getValueCreate(boxName);
  box.name = boxName;
  box.value = arguments.getValueCreate("value");
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
