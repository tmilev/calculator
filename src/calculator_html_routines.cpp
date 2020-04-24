// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "calculator_html_functions.h"

bool CalculatorHtmlFunctions::innerUserInputBox(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorHtmlFunctions::innerUserInputBox");
  MapList<std::string, Expression, MathRoutines::HashString> theArguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementLisT(theCommands, input, theArguments, &theCommands.comments)) {
    return false;
  }
  if (!theArguments.contains("name")) {
    return theCommands << "User input name not specified in: " << input.toString();
  }
  std::string boxName = CalculatorHtmlFunctions::GetUserInputBoxName(input);
  if (theCommands.theObjectContainer.theUserInputTextBoxesWithValues.contains(boxName)) {
    return output.assignValue(theCommands.theObjectContainer.theUserInputTextBoxesWithValues.GetValueCreate(boxName), theCommands);
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
  return output.assignValue(newBox, theCommands);
}

bool CalculatorHtmlFunctions::innerEvaluateSymbols(
  Calculator& theCommands, const Expression& input, Expression& output
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
  theCommands.ParseFillDictionary(theString, theElts);
  Expression evaluatedE;
  std::stringstream out;
  bool previousWasInteger = false;
  for (int i = 0; i < theElts.size; i ++) {
    SyntacticElement& currentElt = theElts[i];
    if (currentElt.controlIndex == theCommands.conVariable()) {
      theCommands.EvaluateExpression(theCommands, currentElt.theData, evaluatedE);
      out << evaluatedE.toString();
      continue;
    }
    if (currentElt.controlIndex == theCommands.conInteger()) {
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
    out << theCommands.controlSequences[currentElt.controlIndex];
  }
  if (previousWasInteger) {
    out << "}";
  }
  return output.assignValue(out.str(), theCommands);
}

bool CalculatorHtmlFunctions::innerSetInputBox(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorHtmlFunctions::innerUserInputBox");
  MapList<std::string, Expression, MathRoutines::HashString> theArguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementLisT(
    theCommands, input, theArguments, &theCommands.comments
  )) {
    return false;
  }
  if (!theArguments.contains("name")) {
    return theCommands << "User input name not specified in: " << input.toString();
  }
  if (!theArguments.contains("value")) {
    return theCommands << "Input box value not specified in: " << input.toString();
  }
  std::string boxName = CalculatorHtmlFunctions::GetUserInputBoxName(input);
  if (theCommands.theObjectContainer.theUserInputTextBoxesWithValues.contains(boxName)) {
    return theCommands << "Input box with name: " << boxName << " already has value.";
  }
  InputBox& theBox =
  theCommands.theObjectContainer.theUserInputTextBoxesWithValues.GetValueCreate(boxName);
  theBox.name = boxName;
  theBox.value = theArguments.GetValueCreate("value");
  std::stringstream out;
  out << "Set value to input box name: " << boxName;
  return output.assignValue(out.str(), theCommands);
}

std::string CalculatorHtmlFunctions::GetUserInputBoxName(const Expression& theBox) {
  MacroRegisterFunctionWithName("CalculatorHtmlFunctions::GetUserInputBoxName");
  if (theBox.owner == nullptr) {
    return "non-initialized-expression";
  }
  Calculator& theCommands = *theBox.owner;
  MapList<std::string, Expression, MathRoutines::HashString> theArguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementLisT(theCommands, theBox, theArguments)) {
    return "corrupt-box";
  }
  if (!theArguments.contains("name")) {
    return "box-without-name";
  }
  std::string theBoxName = "faultyBoxName";
  if (!theArguments.GetValueCreate("name").isOfType<std::string>(&theBoxName)) {
    theBoxName = theArguments.GetValueCreate("name").toString();
  }
  return theBoxName;
}
