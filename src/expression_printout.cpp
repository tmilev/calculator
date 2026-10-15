#include "expression_printout.h"
#include "general_logging_global_variables.h"

ExpressionPrintoutNode& ExpressionPrintoutNode::addChildIndex(int index) {
  this->children.addOnTop(index);
  this->stringSize += this->owner->allPrintouts[index].stringSize;
  return *this;
}

ExpressionPrintoutNode& ExpressionPrintoutNode::addString(
  const std::string& input
) {
  this->checkConsistency();
  this->owner->appendStringTo(input, *this);
  this->stringSize += input.size();
  return *this;
}

bool ExpressionPrintoutNode::checkConsistency() const {
  if (this->owner == nullptr) {
    global.fatal << "Uninitialized expression printout node." << global.fatal;
  }
  return true;
}

ExpressionPrintout& ExpressionPrintoutNode::getOwner() const {
  this->checkConsistency();
  return *this->owner;
}

ExpressionPrintoutNode& ExpressionPrintoutNode::getChild(int childIndex) {
  return this->getOwner().allPrintouts[this->children[childIndex]];
}

void ExpressionPrintoutNode::clear() {
  this->children.clear();
}

bool ExpressionPrintoutNode::isAtomic() const {
  return this->children.size == 0;
}

void ExpressionPrintout::clear() {
  this->allPrintouts.clear();
}

int ExpressionPrintout::addString(const std::string& input) {
  ExpressionPrintoutNode next(*this);
  next.setContent(input);
  int result = this->allPrintouts.size;
  this->allPrintouts.addOnTop(next);
  return result;
}

void ExpressionPrintout::appendStringTo(
  const std::string& input, ExpressionPrintoutNode& appendTo
) {
  appendTo.addChildIndex(this->addString(input));
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
      stack.addOnTop(current.children[i]);
    }
  }
  return out.str();
}
