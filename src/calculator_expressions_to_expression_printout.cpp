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
  ListReferences<Expression> constructed;
  bool useMathML;
  int maximumExpressionDepth;
  ExpressionPrintoutWalker(
    ExpressionPrintout& outputTarget,
    MapReferences<Expression, MathMLAndLatex>& ownerExpressionsWithNotation
  ):
  output(outputTarget),
  expressionsWithNotation(ownerExpressionsWithNotation),
  useMathML(false),
  maximumExpressionDepth(1000) {}
  void initialize(const Expression& input);
  bool maybeConsumeExpressionWithNotation();
  bool maybeConsumeExpressionTooDeep();
  bool maybeIgnore();
  bool consumeNext(const FormatExpressions* format);
  void removeAndPrintStackTop(int newPrintoutIndex);
  bool consumePrintout(ExpressionPrintoutNode& printout);
};

bool ExpressionPrintoutWalker::consumePrintout(
  ExpressionPrintoutNode& printout
) {
  int newPrintoutIndex = output.allPrintouts.size;
  this->output.allPrintouts.addOnTop(printout);
  this->removeAndPrintStackTop(newPrintoutIndex);
  return true;
}

bool ExpressionPrintoutWalker::maybeIgnore() {
  STACK_TRACE("ExpressionPrintoutWalker::maybeIgnore");
  if (this->stack.size == 0) {
    return false;
  }
  ExpressionAndPrintoutData& lastCandidate = *this->stack.lastObject();
  const Expression& lastExpression = *lastCandidate.expression;
  if (!lastExpression.isContext()) {
    return false;
  }
  ExpressionPrintoutNode printout(this->output);
  if (this->output.useMathML) {
    printout.setContent("<ms>Context...</ms>");
  } else {
    printout.setContent("Context");
  }
  return this->consumePrintout(printout);
}

bool ExpressionPrintoutWalker::maybeConsumeExpressionTooDeep() {
  STACK_TRACE("ExpressionPrintoutWalker::maybeConsumeExpressionTooDeep");
  if (this->stack.size <= this->maximumExpressionDepth) {
    return false;
  }
  ExpressionPrintoutNode printout(this->output);
  if (this->output.useMathML) {
    printout.setContent("<ms>...</ms>");
  } else {
    printout.setContent("...");
  }
  return this->consumePrintout(printout);
}

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
  ExpressionPrintoutNode printout(this->output);
  MathMLAndLatex& mathMLAndLatex = expressionsWithNotation.values[index];
  if (this->output.useMathML) {
    printout.setContent(mathMLAndLatex.mathML);
  } else {
    printout.setContent(mathMLAndLatex.latex);
  }
  printout.properties = mathMLAndLatex.properties;
  return this->consumePrintout(printout);
}

void ExpressionPrintoutWalker::removeAndPrintStackTop(int newPrintoutIndex) {
  this->stack.removeLastObject();
  if (this->stack.size == 0) {
    this->output.indexRootNode = newPrintoutIndex;
    return;
  }
  ExpressionAndPrintoutData& current = *this->stack.lastObject();
  current.childrenPrintoutIndices.addOnTop(newPrintoutIndex);
}

void ExpressionPrintoutWalker::initialize(const Expression& input) {
  this->output.clear();
  this->stack.clear();
  if (input.owner != nullptr) {
    this->maximumExpressionDepth = input.owner->maximumRecursionDepth * 2;
  }
  ExpressionPrintoutNode empty(this->output);
  this->output.allPrintouts.addOnTop(empty);
  ExpressionAndPrintoutData inputWithPrintout(input);
  this->stack.addOnTop(inputWithPrintout);
}

bool ExpressionPrintoutWalker::consumeNext(const FormatExpressions* format) {
  STACK_TRACE("ExpressionPrintoutWalker::consumeNext");
  if (this->stack.size == 0) {
    return false;
  }
  ExpressionAndPrintoutData& current = *this->stack.lastObject();
  int totalChildrenProcessed = current.childrenPrintoutIndices.size;
  if (totalChildrenProcessed == 0) {
    Expression transformed;
    bool needsMutationForPrinting =
    current.expression->needsMutationForPrinting(transformed);
    if (needsMutationForPrinting) {
      this->constructed.addOnTop(transformed);
      current.expression = &this->constructed.lastObject();
    }
    // Check whether expression has a shortcut conversion before processing
    // children expressions.
    if (this->maybeConsumeExpressionWithNotation()) {
      return true;
    }
    if (this->maybeConsumeExpressionTooDeep()) {
      return true;
    }
    if (this->maybeIgnore()) {
      return true;
    }
  }
  const Expression& currentExpression = *current.expression;
  if (totalChildrenProcessed < currentExpression.size()) {
    const Expression& nextChildToProcess =
    currentExpression[totalChildrenProcessed];
    // Next child to process.
    this->stack.addOnTop(ExpressionAndPrintoutData(nextChildToProcess));
    return true;
  }
  // All children of the current expression are printed.
  // Print the current one.
  ExpressionPrintoutNode printout(this->output);
  printout.childrenExpressionIndices = current.childrenPrintoutIndices;
  currentExpression.toStringExpressionPrintoutNode(
    printout, this->output, current.childrenPrintoutIndices, format
  );
  return this->consumePrintout(printout);
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
  STACK_TRACE("Expression::toStringExpressionPrintoutNode");
  if (this->owner == nullptr) {
    output.setContent("(Error:NoOwner)");
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
    output.setContent("(ProgrammingError:NotDocumented)");
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
      return handler(*this, output, childIndices, format);
    }
  } else {
    int index =
    this->owner->toStringExpressionPrintoutNodeHandlersAtoms.getIndex(atom);
    if (index >= 0) {
      Expression::ToExpressionPrintoutNodeHandler handler =
      this->owner->toStringExpressionPrintoutNodeHandlersAtoms.values[index];
      return handler(*this, output, childIndices, format);
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
  this->owner->toMathMLExpressionPrintoutNodeHandlersComposite :
  this->owner->toStringExpressionPrintoutNodeHandlersComposite;
  int index = handlers.getIndex(atom);
  if (index < 0) {
    return false;
  }
  Expression::ToExpressionPrintoutNodeHandler handler =
  handlers.values[index];
  return handler(*this, output, childIndices, format);
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
  toStringHandlerCollection.setKeyValue(atom, toStringHandler);
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

void Calculator::addCompositeExpressionPrintoutHandlers(
  int atom,
  Expression::ToExpressionPrintoutNodeHandler toStringHandler,
  Expression::ToExpressionPrintoutNodeHandler toMathMLHandler
) {
  MapList<int, Expression::ToExpressionPrintoutNodeHandler>&
  toStringHandlerCollection =
  this->toStringExpressionPrintoutNodeHandlersComposite;
  MapList<int, Expression::ToExpressionPrintoutNodeHandler>&
  toMathMLHandlerCollection =
  this->toMathMLExpressionPrintoutNodeHandlersComposite;
  if (toStringHandlerCollection.contains(atom)) {
    if (toStringHandlerCollection.getValueNoFail(atom) != toStringHandler) {
      global.fatal
      << "More than one ToExpressionPrintoutNodeHandler for composite "
      << this->operations.keys[atom]
      << "."
      << global.fatal;
    }
  }
  toStringHandlerCollection.setKeyValue(atom, toStringHandler);
  if (toMathMLHandlerCollection.contains(atom)) {
    if (toMathMLHandlerCollection.getValueNoFail(atom) != toMathMLHandler) {
      global.fatal
      << "More than one ToExpressionPrintoutNodeHandler for composite "
      << this->operations.keys[atom]
      << "."
      << global.fatal;
    }
  }
  toMathMLHandlerCollection.setKeyValue(atom, toMathMLHandler);
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
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opIsDenotedBy(),
    Expression::toStringExpressionPrintoutIsDenotedBy,
    Expression::toMathMLExpressionPrintoutIsDenotedBy
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opLog(),
    Expression::toStringExpressionPrintoutLnAbsoluteInsteadOfLogarithm,
    Expression::toMathMLExpressionPrintoutLnAbsoluteInsteadOfLogarithm
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opLogBase(),
    Expression::toStringExpressionPrintoutLogBase,
    Expression::toMathMLExpressionPrintoutLogBase
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opIntervalOpen(),
    Expression::toStringExpressionPrintoutIntervalOpen,
    Expression::toMathMLExpressionPrintoutIntervalOpen
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opIntervalRightClosed(),
    Expression::toStringExpressionPrintoutIntervalRightClosed,
    Expression::toMathMLExpressionPrintoutIntervalRightClosed
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opIntervalLeftClosed(),
    Expression::toStringExpressionPrintoutIntervalLeftClosed,
    Expression::toMathMLExpressionPrintoutIntervalLeftClosed
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opIntervalClosed(),
    Expression::toStringExpressionPrintoutIntervalClosed,
    Expression::toMathMLExpressionPrintoutIntervalClosed
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opQuote(),
    Expression::toStringExpressionPrintoutQuote,
    Expression::toMathMLExpressionPrintoutQuote
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opDefineConditional(),
    Expression::toStringExpressionPrintoutDefineConditional,
    Expression::toMathMLExpressionPrintoutDefineConditional
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opDivide(),
    Expression::toStringExpressionPrintoutDivide,
    Expression::toMathMLExpressionPrintoutDivide
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opTensor(),
    Expression::toStringExpressionPrintoutTensor,
    Expression::toMathMLExpressionPrintoutTensor
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opIn(),
    Expression::toStringExpressionPrintoutIn,
    Expression::toMathMLExpressionPrintoutIn
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opOr(),
    Expression::toStringExpressionPrintoutOr,
    Expression::toMathMLExpressionPrintoutOr
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opAnd(),
    Expression::toStringExpressionPrintoutAnd,
    Expression::toMathMLExpressionPrintoutAnd
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opBinom(),
    Expression::toStringExpressionPrintoutBinom,
    Expression::toMathMLExpressionPrintoutBinom
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opUnderscore(),
    Expression::toStringExpressionPrintoutUnderscore,
    Expression::toMathMLExpressionPrintoutUnderscore
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opSetMinus(),
    Expression::toStringExpressionPrintoutSetMinus,
    Expression::toMathMLExpressionPrintoutSetMinus
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opLimitBoundary(),
    Expression::toStringExpressionPrintoutLimitBoundary,
    Expression::toMathMLExpressionPrintoutLimitBoundary
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opTimes(),
    Expression::toStringExpressionPrintoutTimes,
    Expression::toMathMLExpressionPrintoutTimes
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opCrossProduct(),
    Expression::toStringExpressionPrintoutCrossProduct,
    Expression::toMathMLExpressionPrintoutCrossProduct
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opSqrt(),
    Expression::toStringExpressionPrintoutSqrt,
    Expression::toMathMLExpressionPrintoutSqrt
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opFactorial(),
    Expression::toStringExpressionPrintoutFactorial,
    Expression::toMathMLExpressionPrintoutFactorial
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opAbsoluteValue(),
    Expression::toStringExpressionPrintoutAbsoluteValue,
    Expression::toMathMLExpressionPrintoutAbsoluteValue
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opPower(),
    Expression::toStringExpressionPrintoutPower,
    Expression::toMathMLExpressionPrintoutPower
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opPlus(),
    Expression::toStringExpressionPrintoutPlus,
    Expression::toMathMLExpressionPrintoutPlus
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opDirectSum(),
    Expression::toStringExpressionPrintoutDirectSum,
    Expression::toMathMLExpressionPrintoutDirectSum
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opMinus(),
    Expression::toStringExpressionPrintoutMinus,
    Expression::toMathMLExpressionPrintoutMinus
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opBind(),
    Expression::toStringExpressionPrintoutBind,
    Expression::toMathMLExpressionPrintoutBind
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opEqualEqual(),
    Expression::toStringExpressionPrintoutEqualEqual,
    Expression::toMathMLExpressionPrintoutEqualEqual
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opEqualEqualEqual(),
    Expression::toStringExpressionPrintoutEqualEqualEqual,
    Expression::toMathMLExpressionPrintoutEqualEqualEqual
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opDifferentiate(),
    Expression::toStringExpressionPrintoutDifferentiate,
    Expression::toMathMLExpressionPrintoutDifferentiate
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opDifferential(),
    Expression::toStringExpressionPrintoutDifferential,
    Expression::toMathMLExpressionPrintoutDifferential
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opSum(),
    Expression::toStringExpressionPrintoutSumOrIntegral,
    Expression::toMathMLExpressionPrintoutSumOrIntegral
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opIntegral(),
    Expression::toStringExpressionPrintoutSumOrIntegral,
    Expression::toMathMLExpressionPrintoutSumOrIntegral
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opGreaterThan(),
    Expression::toStringExpressionPrintoutGreaterThan,
    Expression::toMathMLExpressionPrintoutGreaterThan
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opGreaterThanOrEqualTo(),
    Expression::toStringExpressionPrintoutGreaterThanOrEqualTo,
    Expression::toMathMLExpressionPrintoutGreaterThanOrEqualTo
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opLessThanOrEqualTo(),
    Expression::toStringExpressionPrintoutLessThanOrEqualTo,
    Expression::toMathMLExpressionPrintoutLessThanOrEqualTo
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opLimit(),
    Expression::toStringExpressionPrintoutLimit,
    Expression::toMathMLExpressionPrintoutLimit
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opLimitProcess(),
    Expression::toStringExpressionPrintoutLimitProcess,
    Expression::toMathMLExpressionPrintoutLimitProcess
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opLessThan(),
    Expression::toStringExpressionPrintoutLessThan,
    Expression::toMathMLExpressionPrintoutLessThan
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opSequence(),
    Expression::toStringExpressionPrintoutSequence,
    Expression::toMathMLExpressionPrintoutSequence
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opLieBracket(),
    Expression::toStringExpressionPrintoutLieBracket,
    Expression::toMathMLExpressionPrintoutLieBracket
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opMod(),
    Expression::toStringExpressionPrintoutMod,
    Expression::toMathMLExpressionPrintoutMod
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opUnion(),
    Expression::toStringExpressionPrintoutUnion,
    Expression::toMathMLExpressionPrintoutUnion
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opIntersection(),
    Expression::toStringExpressionPrintoutIntersection,
    Expression::toMathMLExpressionPrintoutIntersection
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opUnionNoRepetition(),
    Expression::toStringExpressionPrintoutUnionNoRepetition,
    Expression::toMathMLExpressionPrintoutUnionNoRepetition
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opError(),
    Expression::toStringExpressionPrintoutError,
    Expression::toMathMLExpressionPrintoutError
  );
  toBeInitialized.addAtomExpressionPrintoutHandlers(
    toBeInitialized.opCommandSequence(),
    Expression::toStringExpressionPrintoutEndStatementNested,
    Expression::toMathMLExpressionPrintoutEndStatementNested
  );
  toBeInitialized.addCompositeExpressionPrintoutHandlers(
    toBeInitialized.opMatrix(),
    Expression::toStringExpressionPrintoutMatrix,
    Expression::toMathMLExpressionPrintoutMatrix
  );
}
