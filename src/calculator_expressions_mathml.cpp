#include "calculator_interface.h"

template < >
bool Expression::toMathMLBuiltIn<Rational>(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  out << input.getValue<Rational>().toMathML(format, nullptr);
  return true;
}

bool Expression::toMathMLSequence(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opSequence())) {
    return false;
  }
  out << "<mrow>" << MathML::leftParenthesis;
  for (int i = 1; i < input.size(); i ++) {
    std::string currentChildString = input[i].toMathML();
    out << currentChildString;
    if (i != input.children.size - 1) {
      out << "<mo>,</mo>";
    }
  }
  out << MathML::rightParenthesis;
  if (input.size() == 2) {
    out << "}";
  }
  return true;
}
