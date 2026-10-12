#include "expression_printout.h"

void ExpressionPrintout::clear() {
  this->allPrintouts.clear();
}

int ExpressionPrintout::addString(const std::string& input) {
  ExpressionPrintoutNode next;
  next.content = input;
  int result = this->allPrintouts.size;
  this->allPrintouts.addOnTop(next);
  return result;
}

void ExpressionPrintout::appendStringTo(
  const std::string& input, ExpressionPrintoutNode& appendTo
) {
  appendTo.children.addOnTop(this->addString(input));
}

std::string ExpressionPrintout::toStringRootNode() {
  if (this->indexRootNode < 0) {
    return "(Printout:failure)";
  }
  std::stringstream out;
  List<int> stack;
  stack.addOnTop(this->indexRootNode);
  while (stack.size > 0) {
    ExpressionPrintoutNode& current = this->allPrintouts[*stack.lastObject()];
    stack.removeLastObject();
    if (current.children.size == 0) {
      out << current.content;
      continue;
    }
    for (int i = current.children.size - 1; i >= 0; i --) {
      stack.addOnTop(i);
    }
  }
  return out.str();
}
