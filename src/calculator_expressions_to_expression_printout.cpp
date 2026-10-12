#include "calculator_interface.h"

namespace {

struct ExpressionAndPrintoutData {
public:
  ExpressionAndPrintoutData(): expression(nullptr), printoutIndex(0) {}
  ExpressionAndPrintoutData(const Expression& inputExpression):
  expression(&inputExpression),
  printoutIndex(0) {}
  const Expression* expression;
  int printoutIndex;
  List<int> childrenPrintoutIndices;
};

struct ExpressionPrintoutWalker {
public:
  ExpressionPrintout& output;
  MapReferences<Expression, MathMLAndLatex>& expressionsWithNotation;
  List<ExpressionAndPrintoutData> stack;
  bool useMathML;
  ExpressionPrintoutWalker(
    ExpressionPrintout& outputTarget,
    MapReferences<Expression, MathMLAndLatex>& ownerExpressionsWithNotation
  ):
  output(outputTarget),
  expressionsWithNotation(ownerExpressionsWithNotation),
  useMathML(false) {}
  void initialize(const Expression& input);
  bool maybeConsumeExpressionWithNotation();
  bool consumeNext(const FormatExpressions* format);
  void removeAndPrintStackTop(int newPrintoutIndex);
};

bool ExpressionPrintoutWalker::maybeConsumeExpressionWithNotation() {
  if (this->stack.size == 0) {
    return false;
  }
  ExpressionAndPrintoutData & lastCandidate = *this->stack.lastObject();
  const Expression& lastParent = *lastCandidate.expression;
  int index = expressionsWithNotation.getIndex(lastParent);
  if (index == - 1) {
    // Not an expression with notation.
    return false;
  }
  ExpressionPrintoutNode printout;
  MathMLAndLatex& mathMLAndLatex = expressionsWithNotation.values[index];
  printout.content = mathMLAndLatex.latex;
  printout.properties = mathMLAndLatex.properties;
  int newPrintoutIndex = output.allPrintouts.size;
  this->output.allPrintouts.addOnTop(printout);
  this->removeAndPrintStackTop(newPrintoutIndex);
  return true;
}

void ExpressionPrintoutWalker::removeAndPrintStackTop(int newPrintoutIndex) {
  this->stack.removeLastObject();
  if (this->stack.size == 0) {
    return;
  }
  ExpressionAndPrintoutData& current = *this->stack.lastObject();
  current.childrenPrintoutIndices.addOnTop(newPrintoutIndex);
}

void ExpressionPrintoutWalker::initialize(const Expression& input) {
  this->output.clear();
  this->stack.clear();
  ExpressionPrintoutNode empty;
  this->output.allPrintouts.addOnTop(empty);
  ExpressionAndPrintoutData inputWithPrintout(input);
  this->stack.addOnTop(inputWithPrintout);
}

bool ExpressionPrintoutWalker::consumeNext(const FormatExpressions* format) {
  if (this->stack.size == 0) {
    return false;
  }
  ExpressionAndPrintoutData & current = *this->stack.lastObject();
  const Expression& currentExpression = *current.expression;
  int totalChildrenProcessed = current.childrenPrintoutIndices.size;
  if (totalChildrenProcessed == 0) {
    // Check whether expression has a shortcut conversion before processing
    // children expressions.
    if (this->maybeConsumeExpressionWithNotation()) {
      return true;
    }
  }
  if (totalChildrenProcessed < currentExpression.size()) {
    // Next child to process.
    this->stack.addOnTop(
      ExpressionAndPrintoutData(currentExpression[totalChildrenProcessed])
    );
    return true;
  }
  // All children of the current expression are printed.
  // Print the current one.
  ExpressionPrintoutNode printout;
  currentExpression.toStringExpressionPrintoutNode(
    printout, this->output, current.childrenPrintoutIndices, format
  );
  int indexLast = this->output.allPrintouts.size;
  this->output.allPrintouts.addOnTop(printout);
  this->stack.removeLastObject();
  if (this->stack.size == 0) {
    this->output.indexRootNode = indexLast;
  } else {
    this->stack.lastObject()->childrenPrintoutIndices.addOnTop(indexLast);
  }
  return true;
}
}

void Expression::toExpressionPrintout(
  ExpressionPrintout& output, const FormatExpressions* format
) const {
  STACK_TRACE("Expression::toExpressionPrintout");
  output.clear();
  if (this->owner == nullptr) {
    output.indexRootNode = output.addString("(Error:NoOwner)");
    return;
  }
  ExpressionPrintoutWalker walker(
    output, this->owner->objectContainer.expressionsWithNotation
  );
  walker.initialize(*this);
  while (walker.consumeNext(format)) {}
}

void Expression::toStringExpressionPrintoutNode(
  ExpressionPrintoutNode& output,
  ExpressionPrintout& container,
  List<int>& children,
  const FormatExpressions* format
) const {
  if (this->owner == nullptr) {
    output.content = "(Error:NoOwner)";
    return;
  }
  MemorySaving<FormatExpressions> formatContainer;
  if (format == nullptr) {
    format = &formatContainer.getElement();
    formatContainer.getElement().flagUseQuotes = false;
    formatContainer.getElement().flagExpressionIsInMainDisplay = true;
  }
  RecursionDepthCounter recursionCounter;
  recursionCounter.initialize(&this->owner->recursionDepth);
  this->checkConsistency();
  FormatExpressions formatCopy;
  formatCopy = *format;
  if (
    !this->isOfType<std::string>() &&
    !this->startsWith(this->owner->opCommandSequence())
  ) {
    formatCopy.flagUseQuotes = true;
  }
  if (
    this->toExpressionPrintoutNodeData(
      output, container.useMathML, &formatCopy
    )
  ) {} else if (
    this->toExpressionPrintoutNodeWithAtomHandler(
      output, container, children, &formatCopy
    )
  ) {} else if (
    this->toExpressionPrintoutNodeWithCompositeHandler(
      output, container, children, &formatCopy
    )
  ) {} else if (this->size() == 1) {
    output = container.allPrintouts[children[0]];
  } else if (
    this->toExpressionPrintoutNodeGeneral(
      output, container, children, &formatCopy
    )
  ) {} else {
    output.content = "(ProgrammingError:NotDocumented)";
    // <-not sure if this case is possible
  }
}

bool Expression::toExpressionPrintoutNodeWithAtomHandler(
  ExpressionPrintoutNode& output,
  ExpressionPrintout& container,
  const List<int>& childIndices,
  FormatExpressions* format
) const {
  STACK_TRACE("Expression::toExpressionPrintoutNodeWithAtomHandler");
  if (!this->isList()) {
    return false;
  }
  if (!(*this)[0].isAtom()) {
    return false;
  }
  int atom = (*this)[0].data;
  if (container.useMathML) {
    int index =
    this->owner->toMathMLExpressionPrintoutNodeHandlersAtoms.getIndex(atom);
    if (index >= 0) {
      Expression::ToExpressionPrintoutNodeHandler handler =
      this->owner->toMathMLExpressionPrintoutNodeHandlersAtoms.values[index];
      return handler(*this, output, container, childIndices, format);
    }
  } else {
    int index =
    this->owner->toStringExpressionPrintoutNodeHandlersAtoms.getIndex(atom);
    if (index >= 0) {
      Expression::ToExpressionPrintoutNodeHandler handler =
      this->owner->toStringExpressionPrintoutNodeHandlersAtoms.values[index];
      return handler(*this, output, container, childIndices, format);
    }
  }
  return false;
}

bool Expression::toExpressionPrintoutNodeWithCompositeHandler(
  ExpressionPrintoutNode& output,
  ExpressionPrintout& container,
  const List<int>& childIndices,
  FormatExpressions* format
) const {
  STACK_TRACE("Expression::toExpressionPrintoutNodeWithCompositeHandler");
  if (!this->isList()) {
    return false;
  }
  if (!(*this)[0].isList()) {
    return false;
  }
  if (!(*this)[0][0].isAtom()) {
    return false;
  }
  int atom = (*this)[0][0].data;
  MapList<int, Expression::ToExpressionPrintoutNodeHandler>& handlers =
  container.useMathML ?
  this->owner->toStringExpressionPrintoutNodeHandlersComposite :
  this->owner->toMathMLExpressionPrintoutNodeHandlersComposite;
  int index = handlers.getIndex(atom);
  if (index < 0) {
    return false;
  }
  Expression::ToExpressionPrintoutNodeHandler handler =
  handlers.values[index];
  return handler(*this, output, container, childIndices, format);
}

void Calculator::addAtomExpressionPrintoutHandlers(
  int atom,
  Expression::ToExpressionPrintoutNodeHandler toStringHandler,
  Expression::ToExpressionPrintoutNodeHandler toMathMLHandler
) {
  MapList<int, Expression::ToExpressionPrintoutNodeHandler>&
  toStringHandlerCollection =
  this->toStringExpressionPrintoutNodeHandlersAtoms;
  MapList<int, Expression::ToExpressionPrintoutNodeHandler>&
  toMathMLHandlerCollection =
  this->toMathMLExpressionPrintoutNodeHandlersAtoms;
  if (toStringHandlerCollection.contains(atom)) {
    if (toStringHandlerCollection.getValueNoFail(atom) != toStringHandler) {
      global.fatal
      << "More than one ToExpressionPrintoutNodeHandler for atom "
      << this->operations.keys[atom]
      << "."
      << global.fatal;
    }
  }
  if (toMathMLHandlerCollection.contains(atom)) {
    if (toMathMLHandlerCollection.getValueNoFail(atom) != toMathMLHandler) {
      global.fatal
      << "More than one ToExpressionPrintoutNodeHandler for atom "
      << this->operations.keys[atom]
      << "."
      << global.fatal;
    }
  }
  toMathMLHandlerCollection.setKeyValue(atom, toMathMLHandler);
}

bool Expression::toStringExpressionPrintoutDefine(
  const Expression& input,
  ExpressionPrintoutNode& output,
  ExpressionPrintout& container,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opDefine(), 3)) {
    return false;
  }
  output.properties.needsParenthesesForMultiplicationOnTheRight = true;
  FormatExpressions formatForSubexpressions;
  if (format != nullptr) {
    formatForSubexpressions = *format;
  }
  formatForSubexpressions.flagExpressionIsTopLevel = false;
  if (
    input[1].isListStartingWithAtom(input.owner->opDefine()) ||
    input[1].isListStartingWithAtom(input.owner->opGreaterThan()) ||
    input[1].isListStartingWithAtom(input.owner->opGreaterThanOrEqualTo()) ||
    input[1].isListStartingWithAtom(input.owner->opLessThan()) ||
    input[1].isListStartingWithAtom(input.owner->opLessThanOrEqualTo())
  ) {
    container.appendStringTo("\\left(", output);
    output.children.addOnTop(childrenIndices[1]);
    container.appendStringTo("\\right)", output);
  } else {
    output.children.addOnTop(childrenIndices[1]);
  }
  container.appendStringTo("=", output);
  if (input[2].isListStartingWithAtom(input.owner->opDefine())) {
    container.appendStringTo("\\left(", output);
    output.children.addOnTop(childrenIndices[2]);
    container.appendStringTo("\\right)", output);
  } else {
    output.children.addOnTop(childrenIndices[2]);
  }
  return true;
}

bool Expression::toMathMLExpressionPrintoutDefine(
  const Expression& input,
  ExpressionPrintoutNode& output,
  ExpressionPrintout& container,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opDefine(), 3)) {
    return false;
  }
  FormatExpressions formatForSubexpressions;
  if (format != nullptr) {
    formatForSubexpressions = *format;
  }
  formatForSubexpressions.flagExpressionIsTopLevel = false;
  container.appendStringTo("<mrow>", output);
  if (
    input[1].isListStartingWithAtom(input.owner->opDefine()) ||
    input[1].isListStartingWithAtom(input.owner->opGreaterThan()) ||
    input[1].isListStartingWithAtom(input.owner->opGreaterThanOrEqualTo()) ||
    input[1].isListStartingWithAtom(input.owner->opLessThan()) ||
    input[1].isListStartingWithAtom(input.owner->opLessThanOrEqualTo())
  ) {
    container.appendStringTo(MathML::leftParenthesis, output);
    output.children.addOnTop(childrenIndices[1]);
    container.appendStringTo(MathML::rightParenthesis, output);
  } else {
    output.children.addOnTop(childrenIndices[1]);
  }
  container.appendStringTo("<mo>=</mo>", output);
  if (input[2].isListStartingWithAtom(input.owner->opDefine())) {
    container.appendStringTo(MathML::leftParenthesis, output);
    output.children.addOnTop(childrenIndices[2]);
    container.appendStringTo(MathML::rightParenthesis, output);
  } else {
    output.children.addOnTop(childrenIndices[2]);
  }
  container.appendStringTo("</mrow>", output);
  return true;
}

void Expression::initializeToExpressionPrintoutNodeHandlers(
  Calculator& toBeInitialized
) {
  STACK_TRACE("Expression::initializeToExpressionPrintoutNodeHandlers");
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opDefine(),
    Expression::toStringExpressionPrintoutDefine,
    Expression::toMathMLExpressionPrintoutDefine
  );
  /*
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opIsDenotedBy(), Expression::toMathMLIsDenotedBy
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opLog(), Expression::toMathMLLnAbsoluteInsteadOfLogarithm
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opLogBase(), Expression::toMathMLLogBase
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opIntervalOpen(), Expression::toMathMLIntervalOpen
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opIntervalRightClosed(),
      Expression::toMathMLIntervalRightClosed
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opIntervalLeftClosed(),
      Expression::toMathMLIntervalLeftClosed
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opIntervalClosed(), Expression::toMathMLIntervalClosed
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opQuote(), Expression::toMathMLQuote
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opDefineConditional(),
      Expression::toMathMLDefineConditional
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opDivide(), Expression::toMathMLDivide
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opTensor(), Expression::toMathMLTensor
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opIn(), Expression::toMathMLIn
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opOr(), Expression::toMathMLOr
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opAnd(), Expression::toMathMLAnd
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opBinom(), Expression::toMathMLBinom
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opUnderscore(), Expression::toMathMLUnderscore
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opSetMinus(), Expression::toMathMLSetMinus
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opLimitBoundary(), Expression::toMathMLLimitBoundary
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opTimes(), Expression::toMathMLTimes
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opCrossProduct(), Expression::toMathMLCrossProduct
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opSqrt(), Expression::toMathMLSqrt
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opFactorial(), Expression::toMathMLFactorial
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opAbsoluteValue(), Expression::toMathMLAbsoluteValue
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opPower(), Expression::toMathMLPower
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opPlus(), Expression::toMathMLPlus
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opDirectSum(), Expression::toMathMLDirectSum
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opMinus(), Expression::toMathMLMinus
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opBind(), Expression::toMathMLBind
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opEqualEqual(), Expression::toMathMLEqualEqual
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opEqualEqualEqual(), Expression::toMathMLEqualEqualEqual
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opDifferentiate(), Expression::toMathMLDifferentiate
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opDifferential(), Expression::toMathMLDifferential
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opSum(), Expression::toMathMLSumOrIntegral
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opIntegral(), Expression::toMathMLSumOrIntegral
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opGreaterThan(), Expression::toMathMLGreaterThan
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opGreaterThanOrEqualTo(),
      Expression::toMathMLGreaterThanOrEqualTo
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opLessThanOrEqualTo(),
      Expression::toMathMLLessThanOrEqualTo
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opLimit(), Expression::toMathMLLimit
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opLimitProcess(), Expression::toMathMLLimitProcess
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opLessThan(), Expression::toMathMLLessThan
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opSequence(), Expression::toMathMLSequence
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opLieBracket(), Expression::toMathMLLieBracket
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opMod(), Expression::toMathMLMod
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opUnion(), Expression::toMathMLUnion
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opIntersection(), Expression::toMathMLIntersection
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opUnionNoRepetition(),
      Expression::toMathMLUnionNoRepetition
      );
  toBeInitialized.addOneMathMLAtomHandler(
      toBeInitialized.opError(), Expression::toMathMLError
      );
  toBeInitialized.addOneMathMLCompositeHandler(
      toBeInitialized.opMatrix(), Expression::toMathMLMatrix
      );
  */
}
