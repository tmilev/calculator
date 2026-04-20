#include "calculator_html_functions.h"

bool CalculatorHtmlFunctions::userInputBox(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorHtmlFunctions::userInputBox");
  MapList<std::string, Expression, HashFunctions::hashFunction<std::string> >
  arguments;
  if (
    !CalculatorConversions::loadKeysFromStatementList(
      calculator, input, arguments, false, nullptr, &calculator.comments
    )
  ) {
    return false;
  }
  if (!arguments.contains("name")) {
    return
    calculator
    << "User input name not specified in: "
    << input.toString();
  }
  std::string boxName = CalculatorHtmlFunctions::getUserInputBoxName(input);
  if (
    calculator.objectContainer.userInputTextBoxesWithValues.contains(boxName)
  ) {
    return
    output.assignValue(
      calculator,
      calculator.objectContainer.userInputTextBoxesWithValues.
      getValueCreateEmpty(boxName)
    );
  }
  InputBox newBox;
  newBox.name = boxName;
  for (int i = 0; i < arguments.keys.size; i ++) {
    if (arguments.keys[i] == "value") {
      newBox.value = arguments.values[i];
    }
    if (arguments.keys[i] == "min") {
      newBox.min = arguments.values[i];
    }
    if (arguments.keys[i] == "max") {
      newBox.max = arguments.values[i];
    }
    if (arguments.keys[i] == "step") {
      newBox.step = arguments.values[i];
    }
  }
  return output.assignValue(calculator, newBox);
}

bool CalculatorHtmlFunctions::evaluateSymbols(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorHtmlFunctions::evaluateSymbols");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string argumentString;
  if (!argument.isOfType(&argumentString)) {
    return false;
  }
  ListReferences<SyntacticElement> elements;
  calculator.parser.extractContiguousWords(argumentString, elements);
  Expression evaluatedExpression;
  std::stringstream out;
  bool skipNextVariableInterpretation = false;
  for (int i = 0; i < elements.size; i ++) {
    SyntacticElement& currentElement = elements[i];
    if (currentElement.syntacticRole == SyntacticElement::BACKSLASH) {
      skipNextVariableInterpretation = !skipNextVariableInterpretation;
      out << currentElement.source;
      continue;
    }
    if (currentElement.syntacticRole == SyntacticElement::LETTERS) {
      if (skipNextVariableInterpretation) {
        skipNextVariableInterpretation = false;
        out << currentElement.source;
        continue;
      }
      Expression atom;
      atom.makeAtom(calculator, currentElement.source);
      calculator.evaluateExpression(calculator, atom, evaluatedExpression);
      out << evaluatedExpression.toString();
      continue;
    }
    if (
      currentElement.syntacticRole == SyntacticElement::DIGITS_RAW ||
      currentElement.syntacticRole == SyntacticElement::DIGITS
    ) {
      out << "{";
      out << currentElement.source;
      out << "}";
      continue;
    }
    out << currentElement.source;
  }
  return output.assignValue(calculator, out.str());
}

std::string CalculatorHtmlFunctions::getUserInputBoxName(const Expression& box)
{
  STACK_TRACE("CalculatorHtmlFunctions::getUserInputBoxName");
  if (box.owner == nullptr) {
    return "non-initialized-expression";
  }
  Calculator& calculator = *box.owner;
  MapList<std::string, Expression, HashFunctions::hashFunction<std::string> >
  arguments;
  if (
    !CalculatorConversions::loadKeysFromStatementList(
      calculator, box, arguments, false, nullptr, nullptr
    )
  ) {
    return "corrupt-box";
  }
  if (!arguments.contains("name")) {
    return "box-without-name";
  }
  std::string boxName = "faultyBoxName";
  if (!arguments.getValueCreateEmpty("name").isOfType<std::string>(&boxName)) {
    boxName = arguments.getValueCreateEmpty("name").toString();
  }
  return boxName;
}
