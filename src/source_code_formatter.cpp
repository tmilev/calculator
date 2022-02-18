// The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_file_operations_encodings.h"
#include "general_logging_global_variables.h"
#include "source_code_formatter.h" 
#include "general_strings.h"
#include "web_api.h"

CodeFormatter::Element::Element() {
  this->indentationLevel = 0;
  this->newLinesAfter = 0;
  this->whiteSpaceBefore = 0;
  this->columnFinal = 0;
  this->type = CodeFormatter::Element::Unknown;
  this->owner = nullptr;
  this->parent = nullptr;
  this->indexInParent = - 1;
  this->requiresWhiteSpaceAfter = false;
}

std::string CodeFormatter::Element::toStringWithoutType() const {
  std::stringstream out;
  out << content;
  for (int i = 0; i < this->children.size; i ++) {
    out << this->children[i].toStringWithoutType();
  }
  return out.str();
}

std::string CodeFormatter::Element::toString() const {
  std::stringstream out;
  out << this->toStringWithoutType();
  out << "[" << this->toStringType(this->type) << "]";
  return out.str();
}

std::string CodeFormatter::Element::toStringType(
  CodeFormatter::Element::Type input
) {
  switch (input) {
  case CodeFormatter::Element::TopLevel:
    return "top level";
  case CodeFormatter::Element::CommandList:
    return "command list";
  case CodeFormatter::Element::Command:
    return "command";
  case CodeFormatter::Element::Unknown:
    return "unknown";
  case CodeFormatter::Element::Atom:
    return "atom";
  case CodeFormatter::Element::TypeKeyWord:
    return "type key word";
  case CodeFormatter::Element::Expression:
    return "expression";
  case CodeFormatter::Element::Identifier:
    return "identifier";
  case CodeFormatter::Element::LeftCurlyBrace:
    return "{";
  case CodeFormatter::Element::RightCurlyBrace:
    return "}";
  case CodeFormatter::Element::LeftParenthesis:
    return "(";
  case CodeFormatter::Element::RightParenthesis:
    return ")";
  case CodeFormatter::Element::InParentheses:
    return "InParentheses";
  case CodeFormatter::Element::InBrackets:
    return "InBrackets";
  case CodeFormatter::Element::EndLine:
    return "newline";
  case CodeFormatter::Element::SemiColon:
    return ";";
  case CodeFormatter::Element::Colon:
    return ":";
  case CodeFormatter::Element::DoubleColon:
    return "::";
  case CodeFormatter::Element::Operator:
    return "operator";
  case CodeFormatter::Element::Quote:
    return "quote";
  case CodeFormatter::Element::ConstKeyWord:
    return "const";
  case CodeFormatter::Element::IncludeLine:
    return "includeLine";
  case CodeFormatter::Element::IncludeLineStart:
    return "includeLineStart";
  case CodeFormatter::Element::MacroLine:
    return "macroLine";
  case CodeFormatter::Element::MacroLineStart:
    return "macroLineStart";
  case CodeFormatter::Element::FunctionDeclaration:
    return "functionDeclaration";
  case CodeFormatter::Element::FunctionDefinition:
    return "functionDefinition";
  case CodeFormatter::Element::FunctionWithArguments:
    return "functionWithArguments";
  case CodeFormatter::Element::TypeAndIdentifier:
    return "typeAndIdentifier";
  case CodeFormatter::Element::TypeExpression:
    return "typeExpression";
  case CodeFormatter::Element::CaseKeyWord:
    return "case keyword";
  case CodeFormatter::Element::CaseClauseList:
    return "list case clauses";
  case CodeFormatter::Element::CaseClause:
    return "case clause";
  case CodeFormatter::Element::CaseClauseStart:
    return "case clause start";
  case CodeFormatter::Element::CaseClauseMultipleStart:
    return "case clause multiple start";
  case CodeFormatter::Element::Dummy:
    return "dummy";
  case CodeFormatter::Element::Comment:
    return "comment";
  case CodeFormatter::Element::CommentMultiline:
    return "comment multiline";
  case CodeFormatter::Element::SingleQuote:
    return "single quote";
  case CodeFormatter::Element::ControlKeyWord:
    return "control key word";
  case CodeFormatter::Element::ControlWantsCodeBlock:
    return "control wants code block";
  case CodeFormatter::Element::LeftBracket:
    return "[";
  case CodeFormatter::Element::RightBracket:
    return "]";
  case CodeFormatter::Element::CodeBlock:
    return "code block";
  case CodeFormatter::Element::FileContent:
    return "file content";
  case CodeFormatter::Element::Comma:
    return ",";
  case CodeFormatter::Element::CommaList:
    return "comma list";
  case CodeFormatter::Element::Ampersand:
    return "&";
  case CodeFormatter::Element::Star:
    return "*";
  case CodeFormatter::Element::PublicKeyWord:
    return "public";
  case CodeFormatter::Element::ClassKeyWord:
    return "class";
  case CodeFormatter::Element::PrivateKeyWord:
    return "private";
  case CodeFormatter::Element::VisibilityClause:
    return "visibilityClause";
  case CodeFormatter::Element::DefaultKeyWord:
    return "default";
  case CodeFormatter::Element::ConstructorExternal:
    return "constructorExternal";
  case CodeFormatter::Element::Exclamation:
    return "!";
  case CodeFormatter::Element::If:
    return "if";
  case CodeFormatter::Element::IfWantsCodeBlock:
    return "ifWantsCodeBlock";
  case CodeFormatter::Element::Else:
    return "else";
  case CodeFormatter::Element::IfClause:
    return "ifClause";
  case CodeFormatter::Element::GreaterThan:
    return "greaterThan";
  case CodeFormatter::Element::LessThan:
    return "lessThan";
  case CodeFormatter::Element::CurlyBraceCommaDelimitedList:
    return "CurlyBraceCommaDelimitedList";
  case CodeFormatter::Element::Return:
    return "Return";
  case CodeFormatter::Element::ReturnedExpression:
    return "ReturnedExpression";
  case CodeFormatter::Element::CommentCollection:
    return "CommentCollection";
  }
  return "????";
}

void CodeFormatter::Element::appendExpression(
  const CodeFormatter::Element& other
) {
  this->appendType(other, CodeFormatter::Element::Expression);
}

void CodeFormatter::Element::appendIdentifier(
  const CodeFormatter::Element& other
) {
  this->appendType(other, CodeFormatter::Element::Identifier);
}

void CodeFormatter::Element::appendType(
  const CodeFormatter::Element& other,
  CodeFormatter::Element::Type inputType
) {
  if (this->type == inputType && this->children.size > 0) {
    this->children.addOnTop(other);
    return;
  }
  CodeFormatter::Element thisCopy = *this;
  this->children.clear();
  this->children.addOnTop(thisCopy);
  this->children.addOnTop(other);
  this->content = "";
  this->type = inputType;
}

void CodeFormatter::Element::clear() {
  this->children.clear();
  this->type = CodeFormatter::Element::Unknown;
  this->content = "";
}

void CodeFormatter::Element::makeFrom2(
  CodeFormatter::Element::Type inputType,
  const Element& left,
  const Element& right
) {
  if (this == &left || this == &right) {
    CodeFormatter::Element leftCopy = left;
    CodeFormatter::Element rightCopy = right;
    this->makeFrom2(inputType, leftCopy, rightCopy);
    return;
  }
  this->clear();
  this->type = inputType;
  this->children.addOnTop(left);
  this->children.addOnTop(right);
}

void CodeFormatter::Element::makeFrom1(
  CodeFormatter::Element::Type inputType, const Element& child
) {
  if (this == &child) {
    CodeFormatter::Element childCopy = child;
    this->makeFrom1(inputType, childCopy);
    return;
  }
  this->clear();
  this->type = inputType;
  this->children.addOnTop(child);
}

void CodeFormatter::Element::makeFrom3(
  CodeFormatter::Element::Type inputType,
  const Element& left,
  const Element& middle,
  const Element& right
) {
  if (this == &left || this == &right || this == &middle) {
    CodeFormatter::Element leftCopy = left;
    CodeFormatter::Element middleCopy = middle;
    CodeFormatter::Element rightCopy = right;
    this->makeFrom3(inputType, leftCopy, middleCopy, rightCopy);
    return;
  }
  this->clear();
  this->type = inputType;
  this->children.addOnTop(left);
  this->children.addOnTop(middle);
  this->children.addOnTop(right);
}

void CodeFormatter::Element::makeFrom4(
  CodeFormatter::Element::Type inputType,
  const Element& first,
  const Element& second,
  const Element& third,
  const Element& fourth
) {
  if (
    this == &first || this == &second || this == &third || this == &fourth
  ) {
    CodeFormatter::Element firstCopy = first;
    CodeFormatter::Element secondCopy = second;
    CodeFormatter::Element thirdCopy = third;
    CodeFormatter::Element fourthCopy = fourth;
    this->makeFrom4(
      inputType, firstCopy, secondCopy, thirdCopy, fourthCopy
    );
    return;
  }
  this->clear();
  this->type = inputType;
  this->addChild(first);
  this->addChild(second);
  this->addChild(third);
  this->addChild(fourth);
}

void CodeFormatter::Element::addChild(const CodeFormatter::Element& other) {
  this->children.addOnTop(other);
}

void CodeFormatter::Element::appendCommand(
  const CodeFormatter::Element& other
) {
  this->appendType(other, CodeFormatter::Element::CommandList);
}

bool CodeFormatter::Element::isSuitableForTypeExpression() const {
  return
  this->isIdentifierOrAtom() ||
  this->type == CodeFormatter::Element::TypeExpression ||
  this->type == CodeFormatter::Element::TypeKeyWord;
}

bool CodeFormatter::Element::isSuitableForTemplateArgument() const {
  return
  this->isSuitableForTypeExpression() ||
  this->type == CodeFormatter::Element::CommaList;
}

bool CodeFormatter::Element::isSuitableForParenthesesEnclosure() const {
  return
  this->isCommandListOrCommand() ||
  this->isExpressionIdentifierOrAtom() ||
  this->type == CodeFormatter::Element::TypeAndIdentifier ||
  this->type == CodeFormatter::Element::TypeKeyWord ||
  this->type == CodeFormatter::Element::TypeExpression ||
  this->type == CodeFormatter::Element::CommaList;
}

bool CodeFormatter::Element::isSuitableForCommaListElement() const {
  return
  this->isExpressionIdentifierOrAtom() ||
  this->type == CodeFormatter::Element::TypeAndIdentifier;
}

bool CodeFormatter::Element::isRightDelimiter() const {
  return
  this->type == CodeFormatter::Element::RightBracket ||
  this->type == CodeFormatter::Element::RightCurlyBrace ||
  this->type == CodeFormatter::Element::RightParenthesis;
}

bool CodeFormatter::Element::isIdentifierOrAtom() const {
  return
  this->type == CodeFormatter::Element::Identifier ||
  this->type == CodeFormatter::Element::Atom;
}

bool CodeFormatter::Element::isExpressionOrAtom() const {
  return
  this->type == CodeFormatter::Element::Expression ||
  this->type == CodeFormatter::Element::Atom;
}

bool CodeFormatter::Element::isExpressionIdentifierOrAtom() const {
  return
  this->type == CodeFormatter::Element::Expression ||
  this->type == CodeFormatter::Element::Identifier ||
  this->type == CodeFormatter::Element::Atom;
}

bool CodeFormatter::Element::isExpressionIdentifierAtomOrFunctionWithArguments(
) const {
  return
  this->isExpressionIdentifierOrAtom() ||
  this->type == CodeFormatter::Element::FunctionWithArguments;
}

bool CodeFormatter::Element::isSuitableForTopLevel() const {
  return
  this->type == CodeFormatter::Element::IncludeLine ||
  this->type == CodeFormatter::Element::Command ||
  this->type == CodeFormatter::Element::Comment ||
  this->type == CodeFormatter::Element::CommentMultiline;
}

bool CodeFormatter::Element::isSuitableForCommand() const {
  return
  this->type == CodeFormatter::Element::Expression ||
  this->type == CodeFormatter::Element::FunctionWithArguments ||
  this->type == CodeFormatter::Element::TypeAndIdentifier;
}

bool CodeFormatter::Element::isParenthesesBlock() const {
  return this->type == CodeFormatter::Element::InParentheses;
}

bool CodeFormatter::Element::isCodeBlock() const {
  return this->type == CodeFormatter::Element::CodeBlock;
}

bool CodeFormatter::Element::isCommandListOrCommand() const {
  return
  this->type == CodeFormatter::Element::CommandList ||
  this->type == CodeFormatter::Element::Command;
}

bool CodeFormatter::Element::isCodeBlockOrCommand() const {
  return this->isCodeBlock() || this->type == CodeFormatter::Element::Command;
}

bool CodeFormatter::Element::isComment() const {
  return
  this->type == CodeFormatter::Element::Comment ||
  this->type == CodeFormatter::Element::CommentCollection ||
  this->type == CodeFormatter::Element::CommentMultiline;
}

bool CodeFormatter::Element::isTypeWord() const {
  return this->type == CodeFormatter::Element::TypeKeyWord;
}

bool CodeFormatter::Element::isTypeWordOrTypeExpression() const {
  return
  this->type == CodeFormatter::Element::TypeKeyWord ||
  this->type == CodeFormatter::Element::TypeExpression;
}

bool CodeFormatter::Element::isOperatorOrInequality() const {
  return
  this->isOperator() ||
  this->type == CodeFormatter::Element::GreaterThan ||
  this->type == CodeFormatter::Element::LessThan;
}

bool CodeFormatter::Element::canBeUnaryOnTheRight() const {
  if (!this->isOperator()) {
    return false;
  }
  return this->content == "++" || this->content == "--";
}

bool CodeFormatter::Element::canBeUnaryOnTheLeft() const {
  if (!this->isOperator()) {
    return false;
  }
  return
  this->content == "++" ||
  this->content == "--" ||
  this->content == "&" ||
  this->content == "*" ||
  this->content == "!" ||
  this->content == "-";
}

bool CodeFormatter::Element::isOperator() const {
  return
  this->isStarOrAmpersand() ||
  this->type == CodeFormatter::Element::Operator ||
  this->type == CodeFormatter::Element::Exclamation;
}

bool CodeFormatter::Element::isColonDoubleColonOrComma() const {
  return
  this->type == CodeFormatter::Element::Colon ||
  this->type == CodeFormatter::Element::DoubleColon ||
  this->type == CodeFormatter::Element::Comma;
}

bool CodeFormatter::Element::isColonOrDoubleColon() const {
  return
  this->type == CodeFormatter::Element::Colon ||
  this->type == CodeFormatter::Element::DoubleColon;
}

bool CodeFormatter::Element::isStarOrAmpersand() const {
  return
  this->type == CodeFormatter::Element::Star ||
  this->type == CodeFormatter::Element::Ampersand;
}

bool CodeFormatter::Element::isTypeOrIdentifierOrExpression() const {
  return
  this->type == CodeFormatter::Element::TypeKeyWord ||
  this->type == CodeFormatter::Element::Expression ||
  this->type == CodeFormatter::Element::Identifier ||
  this->type == CodeFormatter::Element::Atom;
}

void CodeFormatter::Element::initializePointers(
  CodeFormatter* inputOwner,
  CodeFormatter::Element* inputParent,
  int inputIndexInParent
) {
  this->owner = inputOwner;
  this->parent = inputParent;
  this->indexInParent = inputIndexInParent;
}

std::string CodeFormatter::Element::format() {
  this->computeIndentation();
  std::stringstream out;
  this->formatDefault(out);
  return out.str();
}

void CodeFormatter::Element::formatDefault(std::stringstream& out) {
  for (int i = 0; i < this->whiteSpaceBefore; i ++) {
    out << " ";
  }
  this->formatContent(out);
  for (int i = 0; i < this->newLinesAfter; i ++) {
    out << "\n";
  }
}

void CodeFormatter::Element::formatContent(std::stringstream& out) {
  if (this->content != "") {
    out << this->content;
    // out << "[" << this->toStringIndentation() << "]";
    return;
  }
  for (int i = 0; i < this->children.size; i ++) {
    this->children[i].formatDefault(out);
  }
}

std::string CodeFormatter::Element::toStringContentOnly() const {
  std::stringstream out;
  this->toStringContentOnly(out);
  return out.str();
}

void CodeFormatter::Element::toStringContentOnly(std::stringstream& out) const {
  if (this->content != "") {
    out << this->content;
    out << " ";
  }
  for (int i = 0; i < this->children.size; i ++) {
    this->children[i].toStringContentOnly(out);
  }
}

std::string CodeFormatter::Element::toStringContentAndMetaData() const {
  std::stringstream out;
  out << "(";
  this->toStringContentOnly(out);
  out << ")";
  out << "[" << this->toStringType(this->type) << "] ";
  out << this->toStringIndentation();
  return out.str();
}

std::string CodeFormatter::Element::toStringIndentation() const {
  std::stringstream out;
  out << " c: " << this->columnFinal;
  out << " a: " << this->newLinesAfter;
  out << " w: " << this->whiteSpaceBefore;
  out << " i: " << this->indentationLevel;
  return out.str();
}

bool CodeFormatter::Element::needsWhiteSpaceBefore() {
  if (this->whiteSpaceBefore > 0) {
    // The parent nodes have already
    // requested white space before.
    return true;
  }
  if (this->content.size() == 0) {
    return false;
  }
  CodeFormatter::Element* previous = this->previousAtom();
  if (previous == nullptr) {
    return false;
  }
  if (previous->content.size() == 0) {
    return false;
  }
  if (previous->newLinesAfter > 0) {
    return false;
  }
  if (previous->content == "return" && this->content != ";") {
    return true;
  }
  char first = this->content[0];
  char lastPrevious = previous->content[previous->content.size() - 1];
  bool startsWithSeparator = this->owner->isSeparatorCharacter(first);
  bool previousEndsWithSeparator =
  this->owner->isSeparatorCharacter(lastPrevious);
  if (!previousEndsWithSeparator) {
    if (!startsWithSeparator) {
      return true;
    }
    if (this->type == CodeFormatter::Element::Quote) {
      return true;
    }
  }
  if (this->owner->preemptsWhitespaceBefore(first)) {
    return false;
  }
  if (lastPrevious == ')' && !startsWithSeparator) {
    return true;
  }
  if (
    this->indexInParent == 0 &&
    this->parent->type == CodeFormatter::Element::InParentheses
  ) {
    CodeFormatter::Element* previousSibling = this->parent->previousSibling();
    if (previousSibling != nullptr) {
      if (
        previousSibling->type == CodeFormatter::Element::TypeExpression &&
        previous->type == CodeFormatter::Element::GreaterThan
      ) {
        return false;
      }
    }
  }
  return previous->requiresWhiteSpaceAfter;
}

void CodeFormatter::Element::computeIndentationLessThan() {
  if (this->parent->type != CodeFormatter::Element::TypeExpression) {
    this->computeIndentationAtomic();
    return;
  }
}

void CodeFormatter::Element::computeIndentationGreaterThan() {
  if (this->parent->type != CodeFormatter::Element::TypeExpression) {
    this->computeIndentationAtomic();
    return;
  }
}

bool CodeFormatter::Element::applyOpenParenthesisException() {
  CodeFormatter::Element* previous = this->previousAtom();
  if (this->content != "(" || previous == nullptr) {
    return false;
  }
  if (previous->newLinesAfter == 0) {
    return false;
  }
  if (previous->columnFinal + 1 >= this->owner->maximumDesiredLineLength) {
    return false;
  }
  if (previous->content == ";" || previous->content == "{") {
    return false;
  }
  // Exception: holds naked ( on a new line is moved to the top line.
  previous->newLinesAfter = 0;
  if (previous->content != "(") {
    this->whiteSpaceBefore = 1;
  }
  return true;
}

void CodeFormatter::Element::computeIndentationAtomic() {
  CodeFormatter::Element* previous = this->previousAtom();
  if (previous != nullptr && this->content == "{") {
    if (
      previous->content == "(" &&
      previous->newLinesAfter > 0 &&
      previous->columnFinal < this->owner->maximumDesiredLineLength
    ) {
      // Exception: formatting of ({ sequence.
      previous->newLinesAfter = 0;
      this->whiteSpaceBefore = 0;
      return;
    }
  }
  if (this->applyOpenParenthesisException()) {
    return;
  }
  if (this->owner->needsSpaceToTheRight(this->content)) {
    this->requiresWhiteSpaceAfter = true;
  }
  if (this->needsWhiteSpaceBefore()) {
    this->whiteSpaceBefore = 1;
  }
  this->columnFinal =
  this->content.size() + this->whiteSpaceBefore + this->offsetFromPrevious();
  if (previous == nullptr) {
    return;
  }
  if (previous->newLinesAfter > 0) {
    this->whiteSpaceBefore = this->indentationLevel;
    this->columnFinal = this->content.size() + this->whiteSpaceBefore;
    return;
  }
  if (this->columnFinal < this->owner->maximumDesiredLineLength) {
    return;
  }
  if (previous->newLinesAfter == 0) {
    previous->newLinesAfter = 1;
  }
  this->whiteSpaceBefore = this->indentationLevel;
  this->columnFinal = this->whiteSpaceBefore + this->content.size();
}

int CodeFormatter::Element::minimalSizeWithSpacebars() {
  if (this->content != "") {
    return this->content.size() + 1;
  }
  int result = 0;
  for (int i = 0; i < this->children.size; i ++) {
    result += this->children[i].minimalSizeWithSpacebars();
  }
  return result;
}

void CodeFormatter::Element::computeIndentationInParentheses() {
  if (this->children.size != 3) {
    this->computeIndentationBasic(0);
    return;
  }
  this->children[0].indentationLevel = this->indentationLevel;
  this->children[0].computeIndentation();
  int middleSize = this->children[1].minimalSizeWithSpacebars();
  if (
    this->children[0].rightMostAtomUnderMe()->columnFinal + middleSize + 1 >
    this->owner->maximumDesiredLineLength
  ) {
    this->children[0].rightMostAtomUnderMe()->newLinesAfter = 1;
    this->children[1].indentationLevel =
    this->indentationLevel + this->owner->tabLength;
    this->children[1].computeIndentation();
    this->children[1].rightMostAtomUnderMe()->newLinesAfter = 1;
  } else {
    this->children[1].computeIndentation();
  }
  this->children[2].indentationLevel = this->indentationLevel;
  this->children[2].computeIndentation();
}

void CodeFormatter::Element::computeIndentationOperator() {
  this->requiresWhiteSpaceAfter = true;
  if (this->content == "->" || this->content == ".") {
    this->requiresWhiteSpaceAfter = false;
  } else if (this->whiteSpaceBefore == 0) {
    this->whiteSpaceBefore = 1;
  }
  this->columnFinal = this->whiteSpaceBefore + this->content.size();
  CodeFormatter::Element* previous = this->previousAtom();
  if (previous == nullptr) {
    return;
  }
  if (previous->newLinesAfter == 0) {
    this->columnFinal += previous->columnFinal;
  }
}

void CodeFormatter::Element::computeIndentationTypeExpression() {
  this->computeIndentationBasic(0);
  this->rightMostAtomUnderMe()->requiresWhiteSpaceAfter = true;
}

bool CodeFormatter::Element::isExpressionLikeForIndentation() {
  return
  this->type == CodeFormatter::Element::Expression ||
  this->type == CodeFormatter::Element::FunctionWithArguments ||
  this->type == CodeFormatter::Element::Atom ||
  this->type == CodeFormatter::Element::Identifier ||
  this->type == CodeFormatter::Element::TypeAndIdentifier;
}

bool CodeFormatter::Element::breakExpressionStringStream() {
  if (!this->isStringStreamExpressionLike()) {
    return false;
  }
  this->resetWhitespaceRecursively();
  for (int i = 0; i < this->children.size; i ++) {
    this->children[i].indentationLevel = this->indentationLevel;
    if (i > 0 && this->children[i].content == "<<") {
      this->children[i].previousAtom()->newLinesAfter = 1;
      this->children[i].whiteSpaceBefore = this->indentationLevel;
    }
    this->children[i].computeIndentation();
  }
  return true;
}

bool CodeFormatter::Element::isStringStreamExpressionLike() {
  if (this->type != CodeFormatter::Element::Expression) {
    return false;
  }
  bool encounteredStream = false;
  for (int i = 1; i < this->children.size; i ++) {
    CodeFormatter::Element& child = this->children[i];
    if (child.type == child.Operator) {
      if (
        child.content != "." && child.content != "->" && child.content != "<<"
      ) {
        return false;
      }
    }
    if (child.content == "<<") {
      encounteredStream = true;
    }
  }
  return encounteredStream;
}

void CodeFormatter::Element::breakExpression() {
  if (this->breakExpressionStringStream()) {
    return;
  }
  this->resetWhitespaceRecursively();
  bool previousIsExpression = false;
  for (int i = 0; i < this->children.size; i ++) {
    CodeFormatter::Element& current = this->children[i];
    current.indentationLevel = this->indentationLevel;
    current.computeIndentation();
    bool breakLine = false;
    if (
      current.isOperator() &&
      previousIsExpression &&
      i != this->children.size - 1
    ) {
      breakLine = true;
    }
    if (breakLine) {
      if (current.content == "." || current.content == "->") {
        breakLine = false;
      }
    }
    if (breakLine) {
      current.newLinesAfter = 1;
    }
    previousIsExpression = current.isExpressionLikeForIndentation();
  }
}

void CodeFormatter::Element::computeIndentationReturnedExpression() {
  this->computeIndentationBasic(0);
  bool shouldBreak = this->containsNewLineAfterRecursively();
  if (
    !shouldBreak &&
    this->rightMostAtomUnderMe()->columnFinal + 1 >=
    this->owner->maximumDesiredLineLength
  ) {
    shouldBreak = true;
  }
  if (!shouldBreak) {
    return;
  }
  if (this->children.size != 2) {
    return;
  }
  CodeFormatter::Element& returned = this->children[0];
  CodeFormatter::Element& expression = this->children[1];
  returned.resetWhitespaceRecursively();
  returned.computeIndentation();
  returned.newLinesAfter = 1;
  expression.breakExpression();
}

void CodeFormatter::Element::computeIndentation() {
  if (this->parent == nullptr) {
    // This must be the top level element.
    for (int i = 0; i < this->children.size; i ++) {
      this->children[i].computeIndentation();
    }
    return;
  }
  if (this->previousAtom() == nullptr) {
    // The first element of code should be a dummy element, no need to
    // compute its indentation.
    return;
  }
  if (this->type == CodeFormatter::Element::TopLevel) {
    this->computeIndentationTopLevel();
    return;
  }
  if (this->type == CodeFormatter::Element::CodeBlock) {
    this->computeIndentationCodeBlock();
    return;
  }
  if (this->type == CodeFormatter::Element::CurlyBraceCommaDelimitedList) {
    this->computeIndentationCurlyBraceCommaDelimitedList();
    return;
  }
  if (this->type == CodeFormatter::Element::ControlWantsCodeBlock) {
    this->computeIndentationControlWantsCodeBlock();
    return;
  }
  if (this->type == CodeFormatter::Element::CaseClause) {
    this->computeIndentationCaseClause();
    return;
  }
  if (this->type == CodeFormatter::Element::CaseClauseStart) {
    this->computeIndentationCaseClauseStart();
    return;
  }
  if (this->type == CodeFormatter::Element::ReturnedExpression) {
    this->computeIndentationReturnedExpression();
    return;
  }
  if (this->type == CodeFormatter::Element::TypeExpression) {
    this->computeIndentationTypeExpression();
    return;
  }
  if (this->type == CodeFormatter::Element::Expression) {
    this->computeIndentationExpression();
    return;
  }
  if (this->type == CodeFormatter::Element::Operator) {
    this->computeIndentationOperator();
    return;
  }
  if (this->type == CodeFormatter::Element::InParentheses) {
    this->computeIndentationInParentheses();
    return;
  }
  if (this->type == CodeFormatter::Element::IfClause) {
    this->computeIndentationIfClause();
    return;
  }
  if (this->type == CodeFormatter::Element::CommaList) {
    this->computeIndentationCommaList();
    return;
  }
  if (this->type == CodeFormatter::Element::Command) {
    this->computeIndentationCommand();
    return;
  }
  if (this->type == CodeFormatter::Element::CommandList) {
    this->computeIndentationCommandList();
    return;
  }
  if (this->type == CodeFormatter::Element::CommentCollection) {
    this->computeIndentationCommentCollection();
    return;
  }
  if (this->type == CodeFormatter::Element::Comment) {
    this->computeIndentationComment();
    return;
  }
  if (this->type == CodeFormatter::Element::FunctionDeclaration) {
    this->computeIndentationFunctionDeclaration();
    return;
  }
  if (this->type == CodeFormatter::Element::FunctionDefinition) {
    this->computeIndentationFunctionDefinition();
    return;
  }
  if (this->type == CodeFormatter::Element::GreaterThan) {
    this->computeIndentationGreaterThan();
    return;
  }
  if (this->type == CodeFormatter::Element::LessThan) {
    this->computeIndentationLessThan();
    return;
  }
  if (this->content != "") {
    this->computeIndentationAtomic();
    return;
  }
  this->computeIndentationBasic(0);
}

int CodeFormatter::Element::offsetFromPrevious() {
  if (this->previousAtom() == nullptr) {
    return this->indentationLevel;
  }
  if (this->previousAtom()->newLinesAfter > 0) {
    return this->indentationLevel;
  }
  return this->previousAtom()->columnFinal;
}

void CodeFormatter::Element::computeIndentationFunctionDeclaration() {
  this->computeIndentationBasic(0);
}

void CodeFormatter::Element::computeIndentationFunctionDefinition() {
  if (this->children.size >= 3) {
    if (
      this->children[2].type == CodeFormatter::Element::ConstKeyWord
    ) {
      this->children[1].rightMostAtomUnderMe()->requiresWhiteSpaceAfter = true;
    }
  }
  CodeFormatter::Element* previous = this->previousAtom();
  if (previous != nullptr) {
    if (previous->newLinesAfter < 2) {
      previous->newLinesAfter = 2;
    }
  }
  this->computeIndentationBasic(0);
  if (this->children.size != 2) {
    return;
  }
  CodeFormatter::Element& lastElementFunctionDeclaration =
  *this->children[0].rightMostAtomUnderMe();
  if (
    lastElementFunctionDeclaration.type == CodeFormatter::Element::ConstKeyWord
  ) {
    lastElementFunctionDeclaration.newLinesAfter = 0;
    this->children[1].leftMostAtomUnderMe()->whiteSpaceBefore = 1;
  }
}

void CodeFormatter::Element::computeIndentationComment() {
  if (this->children.size > 0) {
    return;
  }
  List<std::string> words;
  std::string contentAfterDoubleSlash;
  if (
    !StringRoutines::stringBeginsWith(this->content, "// ") &&
    StringRoutines::stringBeginsWith(
      this->content, "//", &contentAfterDoubleSlash
    )
  ) {
    this->content = "// " + contentAfterDoubleSlash;
  }
  StringRoutines::splitExcludeDelimiter(this->content, ' ', words);
  if (words.size == 0) {
    global << Logger::red << "Unexpected empty comment. " << Logger::endL;
    global.comments << "Unexpected empty comment. ";
    return;
  }
  int currentOffset = this->offsetFromPrevious();
  List<std::string> lines;
  std::string currentWord;
  int maximumWidth = this->owner->maximumDesiredLineLength;
  currentWord = words[0];
  for (int i = 1; i < words.size; i ++) {
    int incomingLength =
    currentOffset + currentWord.size() + 1 + words[i].size();
    if (incomingLength > maximumWidth) {
      if (currentWord != "//") {
        lines.addOnTop(currentWord);
      }
      currentWord = "//";
      currentOffset = this->indentationLevel;
    }
    currentWord += " " + words[i];
  }
  lines.addOnTop(currentWord);
  if (lines.size == 1) {
    this->columnFinal = this->offsetFromPrevious() + 1 + this->content.size();
    this->newLinesAfter = 1;
    return;
  }
  this->content = "";
  this->children.clear();
  this->type = CodeFormatter::Element::CommentCollection;
  this->newLinesAfter = 0;
  for (int i = 0; i < lines.size; i ++) {
    CodeFormatter::Element next;
    next.initializePointers(this->owner, this, i);
    next.indentationLevel = this->indentationLevel;
    next.newLinesAfter = 1;
    next.type = CodeFormatter::Element::Comment;
    next.content = lines[i];
    this->addChild(next);
  }
}

void CodeFormatter::Element::computeIndentationCurlyBraceCommaDelimitedList() {
  this->computeIndentationBasic(0);
  if (this->children.size < 3) {
    return;
  }
  if (!this->containsNewLineAfterRecursively()) {
    return;
  }
  this->resetWhitespaceRecursively();
  this->children[0].indentationLevel = this->indentationLevel;
  this->children[0].computeIndentation();
  this->children[0].newLinesAfter = 1;
  int indentationChildren = this->indentationLevel + this->owner->tabLength;
  for (int i = 1; i < this->children.size - 1; i ++) {
    this->children[i].indentationLevel = indentationChildren;
    this->children[i].computeIndentation();
    if (
      this->children[i].content == "," || i == this->children.size - 2
    ) {
      this->children[i].newLinesAfter = 1;
    }
  }
  this->children.lastObject()->indentationLevel = this->indentationLevel;
  this->children.lastObject()->computeIndentation();
  this->children.lastObject()->whiteSpaceBefore = this->indentationLevel;
}

void CodeFormatter::Element::computeIndentationCodeBlock() {
  if (this->children.size < 2) {
    this->computeIndentationBasic(0);
    return;
  }
  this->children[0].whiteSpaceBefore = 1;
  this->children[0].indentationLevel = this->indentationLevel;
  this->children[0].computeIndentation();
  if (this->children.size == 3) {
    if (
      this->children[1].type == CodeFormatter::Element::CommandList
    ) {
      this->children[0].rightMostAtomUnderMe()->newLinesAfter = 1;
    }
    if (this->children[1].type == CodeFormatter::Element::Command) {
      this->children[0].newLinesAfter = 1;
      this->children[1].rightMostAtomUnderMe()->newLinesAfter = 1;
    }
  }
  int indentationChildren = this->indentationLevel + this->owner->tabLength;
  for (int i = 1; i < this->children.size - 1; i ++) {
    this->children[i].indentationLevel = indentationChildren;
    this->children[i].computeIndentation();
  }
  this->children.lastObject()->indentationLevel = this->indentationLevel;
  this->children.lastObject()->computeIndentation();
}

void CodeFormatter::Element::computeIndentationControlWantsCodeBlock() {
  if (this->children.size != 2) {
    this->computeIndentationBasic(0);
    return;
  }
  this->children[0].indentationLevel = this->indentationLevel;
  this->children[1].indentationLevel = this->indentationLevel;
  this->children[0].requiresWhiteSpaceAfter = true;
  this->children[0].computeIndentation();
  this->children[1].computeIndentation();
}

void CodeFormatter::Element::computeIndentationCaseClause() {
  for (int i = 0; i < this->children.size; i ++) {
    this->children[i].previousAtom()->newLinesAfter = 1;
    this->children[i].indentationLevel = this->indentationLevel;
    this->children[i].computeIndentation();
  }
  if (this->children.size > 0) {
    this->children.lastObject()->rightMostAtomUnderMe()->newLinesAfter = 1;
  }
}

void CodeFormatter::Element::computeIndentationCaseClauseStart() {
  this->indentationLevel -= this->owner->tabLength;
  if (this->indentationLevel < 0) {
    this->indentationLevel = 0;
  }
  this->computeIndentationBasic(0);
}

std::string CodeFormatter::Element::toStringHTMLTree() {
  std::stringstream out;
  out
  << "<div style='border-left-style: solid; margin-left: 10px;'>"
  << this->content
  << "["
  << this->toStringType(this->type)
  << "]"
  << " i: "
  << this->indentationLevel;
  if (this->children.size > 1) {
    out << " c: " << this->children.size;
  }
  if (this->whiteSpaceBefore > 0) {
    out << " w: " << this->whiteSpaceBefore;
  }
  if (this->requiresWhiteSpaceAfter) {
    out << " r: " << this->requiresWhiteSpaceAfter;
  }
  for (int i = 0; i < this->newLinesAfter; i ++) {
    out << "&darr;";
  }
  for (int i = 0; i < this->children.size; i ++) {
    out << this->children[i].toStringHTMLTree();
  }
  out << "</div>";
  return out.str();
}

void CodeFormatter::Element::computeIndentationCommentCollection() {
  for (int i = 0; i < this->children.size; i ++) {
    CodeFormatter::Element& current = this->children[i];
    current.indentationLevel = this->indentationLevel;
    current.computeIndentation();
    current.rightMostAtomUnderMe()->newLinesAfter = 1;
    current.leftMostAtomUnderMe()->whiteSpaceBefore = current.indentationLevel;
  }
  //  this->computeIndentationBasic(0);
}

void CodeFormatter::Element::computeIndentationCommandList() {
  if (this->parent->type != CodeFormatter::Element::CodeBlock) {
    for (int i = 0; i < this->children.size; i ++) {
      CodeFormatter::Element& current = this->children[i];
      if (i != this->children.size - 1) {
        current.rightMostAtomUnderMe()->requiresWhiteSpaceAfter = true;
      }
      current.indentationLevel = this->indentationLevel;
      current.computeIndentation();
    }
    return;
  }
  for (CodeFormatter::Element& current : this->children) {
    current.rightMostAtomUnderMe()->newLinesAfter = 1;
    current.indentationLevel = this->indentationLevel;
    if (current.type == CodeFormatter::Element::Comment) {
      current.whiteSpaceBefore = this->indentationLevel;
    }
    current.computeIndentation();
  }
}

void CodeFormatter::Element::resetWhitespaceRecursively() {
  this->newLinesAfter = 0;
  this->whiteSpaceBefore = 0;
  this->requiresWhiteSpaceAfter = false;
  this->columnFinal = 0;
  for (int i = 0; i < this->children.size; i ++) {
    this->children[i].resetWhitespaceRecursively();
  }
}

bool CodeFormatter::Element::containsNewLineAfterRecursively() {
  for (int i = 0; i < this->children.size; i ++) {
    if (this->children[i].containsNewLineAfterRecursively()) {
      return true;
    }
  }
  return this->newLinesAfter > 0;
}

void CodeFormatter::Element::computeIndentationIfClause() {
  this->computeIndentationBasic(0);
  for (int i = 0; i < this->children.size; i ++) {
    if (this->children[i].type == CodeFormatter::Element::Else) {
      this->children[i].whiteSpaceBefore = 1;
    }
  }
}

void CodeFormatter::Element::computeIndentationCommand() {
  MacroRegisterFunctionWithName(
    "CodeFormatter::Element::computeIndentationCommand"
  );
  this->computeIndentationBasic(0);
}

void CodeFormatter::Element::computeIndentationCommaList() {
  bool mustSplitLines = false;
  if (
    this->minimalSizeWithSpacebars() + this->offsetFromPrevious() >
    this->owner->maximumDesiredLineLength
  ) {
    mustSplitLines = true;
  }
  if (mustSplitLines) {
    this->previousAtom()->newLinesAfter = 1;
  }
  for (int i = 0; i < this->children.size; i ++) {
    CodeFormatter::Element& current = this->children[i];
    if (current.content == ",") {
      if (mustSplitLines) {
        current.rightMostAtomUnderMe()->newLinesAfter = 1;
      }
      current.requiresWhiteSpaceAfter = true;
    }
    current.indentationLevel = this->indentationLevel;
    current.computeIndentation();
  }
}

void CodeFormatter::Element::computeIndentationExpression() {
  this->computeIndentationBasic(0);
  if (!this->containsNewLineAfterRecursively()) {
    return;
  }
  this->breakExpression();
}

void CodeFormatter::Element::computeIndentationBasic(int startingIndex) {
  for (int i = startingIndex; i < this->children.size; i ++) {
    this->children[i].indentationLevel = this->indentationLevel;
    this->children[i].computeIndentation();
  }
}

void CodeFormatter::Element::computeIndentationTopLevel() {
  MacroRegisterFunctionWithName(
    "CodeFormatter::Element::computeIndentationTopLevel"
  );
  for (int i = 0; i < this->children.size; i ++) {
    CodeFormatter::Element& current = this->children[i];
    if (current.type == CodeFormatter::Element::Dummy) {
      continue;
    }
    current.indentationLevel = 0;
    current.whiteSpaceBefore = 0;
    CodeFormatter::Element* rightMostAtom = current.rightMostAtomUnderMe();
    rightMostAtom->newLinesAfter = 1;
    current.computeIndentation();
    bool addExtraLine = false;
    if (i < this->children.size - 1) {
      CodeFormatter::Element& next = this->children[i + 1];
      if (
        current.type == CodeFormatter::Element::IncludeLine &&
        next.type != CodeFormatter::Element::IncludeLine
      ) {
        addExtraLine = true;
      }
    } else {
      addExtraLine = true;
    }
    if (addExtraLine) {
      rightMostAtom->newLinesAfter = 2;
    }
  }
}

CodeFormatter::Element* CodeFormatter::Element::previousSibling() {
  if (this->parent == nullptr || this->indexInParent <= 0) {
    return nullptr;
  }
  return &this->parent->children[this->indexInParent - 1];
}

CodeFormatter::Element* CodeFormatter::Element::previousAtom() {
  if (this->parent == nullptr) {
    return nullptr;
  }
  if (this->indexInParent == 0) {
    return this->parent->previousAtom();
  }
  return
  this->parent->children[this->indexInParent - 1].rightMostAtomUnderMe();
}

CodeFormatter::Element* CodeFormatter::Element::rightMostAtomUnderMe() {
  if (this->children.size == 0) {
    return this;
  }
  return this->children.lastObject()->rightMostAtomUnderMe();
}

CodeFormatter::Element* CodeFormatter::Element::leftMostAtomUnderMe() {
  if (this->children.size == 0) {
    return this;
  }
  return this->children[0].leftMostAtomUnderMe();
}

bool CodeFormatter::Words::previousIsForwardSlash() {
  if (this->elements.size == 0) {
    return false;
  }
  return this->elements.lastObject()->content == "/";
}

bool CodeFormatter::Words::processSeparatorCharacters() {
  if (!this->owner->isSeparatorCharacter(this->currentChar)) {
    this->currentWord.content += this->currentChar;
    return true;
  }
  this->addCurrentWord();
  this->currentWord.content = this->currentChar;
  if (this->currentChar == '\"') {
    this->currentWord.type = CodeFormatter::Element::Quote;
    this->flagPreviousIsStandaloneBackSlash = false;
    return true;
  }
  if (this->currentChar == '\'') {
    this->currentWord.type = CodeFormatter::Element::SingleQuote;
    this->flagPreviousIsStandaloneBackSlash = false;
    return true;
  }
  if (this->currentChar == '/' && this->previousIsForwardSlash()) {
    this->elements.removeLastObject();
    this->currentWord.content = "//";
    this->currentWord.type = CodeFormatter::Element::Comment;
    return true;
  }
  if (this->currentChar == '*' && this->previousIsForwardSlash()) {
    this->elements.removeLastObject();
    this->currentWord.content = "/*";
    this->currentWord.type = CodeFormatter::Element::CommentMultiline;
    return true;
  }
  this->addCurrentWord();
  return true;
}

bool CodeFormatter::Words::processCharacterInQuotes() {
  if (this->currentWord.type != CodeFormatter::Element::Quote) {
    return false;
  }
  this->currentWord.content += this->currentChar;
  if (
    this->currentChar == '"' && !this->flagPreviousIsStandaloneBackSlash
  ) {
    this->addCurrentWord();
    return true;
  }
  if (this->currentChar == '\\') {
    this->flagPreviousIsStandaloneBackSlash =
    !this->flagPreviousIsStandaloneBackSlash;
  } else {
    this->flagPreviousIsStandaloneBackSlash = false;
  }
  return true;
}

bool CodeFormatter::Words::processCharacterInSingleQuotes() {
  if (this->currentWord.type != CodeFormatter::Element::SingleQuote) {
    return false;
  }
  this->currentWord.content += this->currentChar;
  if (
    this->currentChar == '\'' && !this->flagPreviousIsStandaloneBackSlash
  ) {
    this->addCurrentWord();
    return true;
  }
  if (this->currentChar == '\\') {
    this->flagPreviousIsStandaloneBackSlash =
    !this->flagPreviousIsStandaloneBackSlash;
  } else {
    this->flagPreviousIsStandaloneBackSlash = false;
  }
  return true;
}

bool CodeFormatter::Words::processCharacterInComment() {
  if (this->currentWord.type != CodeFormatter::Element::Comment) {
    return false;
  }
  if (this->currentChar == '\n') {
    this->addCurrentWord();
    return this->processSeparatorCharacters();
  }
  this->currentWord.content += this->currentChar;
  return true;
}

bool CodeFormatter::Words::processCharacterInMultilineComment() {
  if (this->currentWord.type != CodeFormatter::Element::Comment) {
    return false;
  }
  this->currentWord.content += this->currentChar;
  if (
    this->currentChar == '/' &&
    this->currentWord.content[this->currentWord.content.size() - 1] == '*'
  ) {
    this->addCurrentWord();
  }
  return true;
}

bool CodeFormatter::Words::extractCodeElements(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CodeFormatter::Words::extractCodeElements");
  (void) comments;
  std::string& inputCode = this->owner->inputCode;
  this->elements.setExpectedSize(
    static_cast<signed>(inputCode.size())
  );
  this->flagPreviousIsStandaloneBackSlash = false;
  for (unsigned i = 0; i < inputCode.size(); i ++) {
    this->currentChar = inputCode[i];
    if (this->processCharacterInQuotes()) {
      continue;
    }
    if (this->processCharacterInSingleQuotes()) {
      continue;
    }
    if (this->processCharacterInComment()) {
      continue;
    }
    if (this->processCharacterInMultilineComment()) {
      continue;
    }
    if (this->processSeparatorCharacters()) {
      continue;
    }
  }
  this->addCurrentWord();
  return true;
}

CodeFormatter::Words::Words() {
  this->flagPreviousIsStandaloneBackSlash = false;
  this->currentChar = 0;
}

void CodeFormatter::Words::addCurrentWord() {
  this->flagPreviousIsStandaloneBackSlash = false;
  if (this->currentWord.content == "") {
    return;
  }
  if (this->currentWord.content == "\n") {
    this->currentWord.type = CodeFormatter::Element::Type::EndLine;
  }
  this->elements.addOnTop(this->currentWord);
  this->currentWord.type = CodeFormatter::Element::Type::Unknown;
  this->currentWord.content = "";
}

void CodeFormatter::Words::initialize(CodeFormatter& inputOwner) {
  this->owner = &inputOwner;
}

bool CodeFormatter::initializeFileNames(
  const std::string& fileName,
  const std::string& inputOutputFileNameEmptyForAuto,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::initializeFileNames");
  this->inputFileName = fileName;
  if (
    !FileOperations::loadFileToStringVirtual(
      this->inputFileName, this->inputCode, false, comments
    )
  ) {
    if (comments != nullptr) {
      *comments << "Failed to load file. ";
    }
    return false;
  }
  if (inputOutputFileNameEmptyForAuto != "") {
    this->outputFileName = inputOutputFileNameEmptyForAuto;
  } else {
    this->outputFileName = this->inputFileName + ".new";
  }
  return true;
}

bool CodeFormatter::isLetterLike(char input) {
  return !this->isSeparatorCharacter(input);
}

bool CodeFormatter::isIdentifierWord(const std::string& input) {
  for (unsigned i = 0; i < input.size(); i ++) {
    char current = input[i];
    if (
      !MathRoutines::isDigit(current) &&
      !MathRoutines::isLatinLetter(current) &&
      current != '_'
    ) {
      return false;
    }
  }
  return true;
}

std::string CodeFormatter::toStringLinks() {
  MacroRegisterFunctionWithName("SourceCodeFormatter::toStringLinks");
  std::stringstream out;
  out
  << "<a href = '"
  << this->inputFileName
  << "'>"
  << this->inputFileName
  << "</a>, ";
  out
  << "<a href = '"
  << this->outputFileName
  << "'>"
  << this->outputFileName
  << "</a>. ";
  return out.str();
}

bool CodeFormatter::formatCPPDirectory(
  const std::string& inputDirectory, std::stringstream* comments
) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::formatCPPDirectory");
  std::string directory = inputDirectory;
  if (directory == "") {
    if (comments != nullptr) {
      *comments << "Format cpp directory: empty directory name not allowed. ";
    }
    return false;
  }
  if (directory[directory.size() - 1] != '/') {
    directory += "/";
  }
  List<std::string> allFiles,newFileNames,oldFileNames,allFileExtensions;
  if (
    !FileOperations::getFolderFileNamesVirtual(
      directory, allFiles, &allFileExtensions, false, false, comments
    )
  ) {
    return false;
  }
  newFileNames.setExpectedSize(allFiles.size);
  oldFileNames.setExpectedSize(allFiles.size);
  for (int i = 0; i < allFiles.size; i ++) {
    if (allFileExtensions[i] == ".cpp" || allFileExtensions[i] == ".h") {
      oldFileNames.addOnTop(directory + allFiles[i]);
      newFileNames.addOnTop(directory + "new/" + allFiles[i]);
    }
  }
  for (int i = 0; i < newFileNames.size; i ++) {
    CodeFormatter formatter;
    if (
      !formatter.formatCPPSourceCode(
        oldFileNames[i], newFileNames[i], comments, false
      )
    ) {
      return false;
    }
  }
  return true;
}

bool CodeFormatter::formatCPPSourceCode(
  const std::string& inputFileName,
  const std::string& inputOutputFileNameEmptyForAuto,
  std::stringstream* comments,
  bool logDebugInfo
) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::formatCPPSourceCode");
  if (
    !this->initializeFileNames(
      inputFileName, inputOutputFileNameEmptyForAuto, comments
    )
  ) {
    return false;
  }
  this->words.initialize(*this);
  if (!this->words.extractCodeElements(comments)) {
    return false;
  }
  this->processor.initialize(*this);
  this->processor.flagPrepareReport = logDebugInfo;
  this->processor.consumeElements();
  std::stringstream out;
  this->normalizeBinaryOperationsRecursively(this->processor.code);
  this->wirePointersRecursively(this->processor.code, nullptr, - 1);
  if (logDebugInfo) {
    global.comments
    << "<b>Parsing log follows.</b><br>"
    << this->processor.debugLog;
  }
  if (this->processor.code.children.size <= this->dummyElements + 1) {
    out << this->processor.code.format();
  } else {
    out << "FAILED_TO_PARSE;\n";
    for (
      int i = this->dummyElements; i < this->processor.code.children.size; i ++
    ) {
      this->processor.code.children[i].toStringContentOnly(out);
      out << "\n";
    }
  }
  if (logDebugInfo) {
    global.comments
    << "<hr>Code element tree follows<br>"
    << this->processor.code.toStringHTMLTree();
  }
  this->transformedContent = out.str();
  if (!this->writeFormatedCode(comments)) {
    return false;
  }
  if (comments != nullptr) {
    (*comments) << this->toStringLinks();
  }
  return true;
}

bool CodeFormatter::needsSpaceToTheRight(const std::string& word) {
  return this->needsSpaceToTheRightContainer.contains(word);
}

bool CodeFormatter::preemptsWhitespaceBefore(char input) {
  return input == ')';
}

bool CodeFormatter::isSeparatorCharacter(char input) {
  unsigned char inputUnsigned = static_cast<unsigned char>(input);
  return this->separatorCharactersMap[inputUnsigned];
}

bool CodeFormatter::isSeparator(const std::string& input) {
  if (input.size() != 1) {
    return false;
  }
  return this->isSeparatorCharacter(input[0]);
}

bool CodeFormatter::isOperator(const std::string& input) {
  return this->operatorList.contains(input);
}

void CodeFormatter::addOperatorOverride(
  const std::string& overridingOperator,
  const List<std::string>& overridden
) {
  HashedList<std::string, HashFunctions::hashFunction>& overridenHashed =
  this->operatorOverrides.getValueCreateEmpty(overridingOperator);
  overridenHashed.addListOnTop(overridden);
}

CodeFormatter::CodeFormatter() {
  this->separatorCharacters = "(){} \t\n\r,:;.*&+-/%[]#\"'=><!|";
  this->doesntNeedSpaceToTheLeft = "&,:.()";
  this->doesntNeedSpaceToTheRight = "():.";
  this->pairsNotSeparatedBySpace.addOnTop(
    List<std::string>({"&", "&"})
  );
  this->pairsNotSeparatedBySpace.addOnTop(
    List<std::string>({"&", "*"})
  );
  this->pairsNotSeparatedBySpace.addOnTop(
    List<std::string>({"*", "&"})
  );
  List<std::string> relations = List<std::string>({"=", "==", ">=", ">", "<=", "!="});
  List<std::string> andAndOr = List<std::string>({"&&", "||"});
  List<std::string> arithmeticOperations = List<std::string>({"+", "-", "/", "*"});
  List<std::string> bitShift = List<std::string>({"<<", ">>"});
  List<std::string> andOrRelations = relations;
  andOrRelations.addListOnTop(andAndOr);
  this->addOperatorOverride("+", andOrRelations);
  // The negative sign may extend to the -> operator, so
  // its presence on the right overrides the other arithmetic operators.
  this->addOperatorOverride("-", andOrRelations);
  this->addOperatorOverride("-", arithmeticOperations);
  this->addOperatorOverride("-", bitShift);
  this->addOperatorOverride("->", andOrRelations);
  this->addOperatorOverride("->", arithmeticOperations);
  this->addOperatorOverride("->", bitShift);
  this->addOperatorOverride(".", andOrRelations);
  this->addOperatorOverride(".", arithmeticOperations);
  this->addOperatorOverride(".", bitShift);
  this->addOperatorOverride("==", andAndOr);
  this->addOperatorOverride("=", andAndOr);
  this->addOperatorOverride(">=", andAndOr);
  this->addOperatorOverride("<=", andAndOr);
  this->addOperatorOverride(">", andAndOr);
  this->addOperatorOverride(
    "<", List<std::string>({"&&", "||", "="})
  );
  this->addOperatorOverride("!=", andAndOr);
  this->addOperatorOverride("!", andAndOr);
  this->addOperatorOverride("||", List<std::string>({"->", "."}));
  this->addOperatorOverride("&&", List<std::string>({"->", "."}));
  this->elementTypes.setKeyValue(":", CodeFormatter::Element::Colon);
  this->elementTypes.setKeyValue(";", CodeFormatter::Element::SemiColon);
  this->elementTypes.setKeyValue(
    "(", CodeFormatter::Element::LeftParenthesis
  );
  this->elementTypes.setKeyValue(
    ")", CodeFormatter::Element::RightParenthesis
  );
  this->elementTypes.setKeyValue(
    "{", CodeFormatter::Element::LeftCurlyBrace
  );
  this->elementTypes.setKeyValue(
    "}", CodeFormatter::Element::RightCurlyBrace
  );
  this->elementTypes.setKeyValue("[", CodeFormatter::Element::LeftBracket);
  this->elementTypes.setKeyValue(
    "]", CodeFormatter::Element::RightBracket
  );
  this->elementTypes.setKeyValue("&", CodeFormatter::Element::Ampersand);
  this->elementTypes.setKeyValue("*", CodeFormatter::Element::Star);
  this->elementTypes.setKeyValue("!", CodeFormatter::Element::Exclamation);
  this->elementTypes.setKeyValue(",", CodeFormatter::Element::Comma);
  this->elementTypes.setKeyValue(">", CodeFormatter::Element::GreaterThan);
  this->elementTypes.setKeyValue("<", CodeFormatter::Element::LessThan);
  this->elementTypes.setKeyValue(
    "const", CodeFormatter::Element::ConstKeyWord
  );
  this->elementTypes.setKeyValue(
    "case", CodeFormatter::Element::CaseKeyWord
  );
  this->elementTypes.setKeyValue(
    "default", CodeFormatter::Element::DefaultKeyWord
  );
  this->elementTypes.setKeyValue("if", CodeFormatter::Element::If);
  this->elementTypes.setKeyValue("else", CodeFormatter::Element::Else);
  this->elementTypes.setKeyValue("return", CodeFormatter::Element::Return);
  this->elementTypes.setKeyValue("public", CodeFormatter::Element::PublicKeyWord);
  this->elementTypes.setKeyValue("private", CodeFormatter::Element::PrivateKeyWord);
  this->elementTypes.setKeyValue("class", CodeFormatter::Element::ClassKeyWord);
  this->typeKeyWords.addListOnTop(
    List<std::string>({"bool", "void", "char", "unsigned"})
  );
  this->controlKeyWords.addListOnTop(
    List<std::string>({"for", "while", "switch", })
  );
  this->needsSpaceToTheRightContainer.addListOnTop(
    List<std::string>({"for", "if", "while"})
  );
  this->operatorList.addListOnTop(
    List<std::string>({
        "+",
        "++",
        "-",
        "--",
        ">",
        "<",
        "=",
        "/",
        "*",
        "|",
        "%",
        ".",
        "->",
        "==",
        ">=",
        "<=",
        "!=",
        "*=",
        "/=",
        "%=",
        "+=",
        "-=",
        ">>",
        "<<",
        "&&",
        "||"
      }
    )
  );
  this->maximumDesiredLineLength = 80;
  this->indexCurrentlyConsumed = 0;
  this->separatorCharactersMap.initializeFillInObject(256, false);
  for (unsigned i = 0; i < this->separatorCharacters.size(); i ++) {
    this->separatorCharactersMap[static_cast<signed>(this->separatorCharacters[i])] = true;
  }
  for (unsigned i = 0; i < this->doesntNeedSpaceToTheLeft.size(); i ++) {
    std::string incoming = this->doesntNeedSpaceToTheLeft.substr(i, 1);
    this->doesntNeedSpaceToTheLeftContainer.addOnTop(incoming);
  }
  for (
    unsigned i = 0; i < this->doesntNeedSpaceToTheRight.size(); i ++
  ) {
    std::string incoming = this->doesntNeedSpaceToTheRight.substr(i, 1);
    this->doesntNeedSpaceToTheRightContainer.addOnTop(incoming);
  }
}

CodeFormatter::Processor::Processor() {
  this->owner = nullptr;
  this->flagPrepareReport = false;
  this->flagExceededReportSize = false;
}

void CodeFormatter::Processor::initialize(CodeFormatter& inputOwner) {
  this->owner = &inputOwner;
}

void CodeFormatter::Processor::consumeElements() {
  List<CodeFormatter::Element>& elements = this->owner->words.elements;
  this->stack.clear();
  for (int i = 0; i < this->owner->dummyElements; i ++) {
    CodeFormatter::Element element;
    element.type = CodeFormatter::Element::Dummy;
    this->stack.addOnTop(element);
  }
  CodeFormatter::Element topLevel;
  topLevel.type = CodeFormatter::Element::TopLevel;
  this->stack.addOnTop(topLevel);
  for (int i = 0; i < elements.size; i ++) {
    this->consumeOneElement(elements[i]);
  }
  this->code.type = CodeFormatter::Element::FileContent;
  this->code.content = "";
  this->code.children = this->stack;
}

void CodeFormatter::Processor::consumeOneElement(
  CodeFormatter::Element& incoming
) {
  MacroRegisterFunctionWithName("CodeFormatter::Processor::consumeOneElement");
  this->lastRuleName = "consume";
  this->stack.addOnTop(incoming);
  this->appendLog();
  int stackSizeToImprove = this->stack.size;
  int rulesSinceImprovement = 0;
  while (this->applyOneRule()) {
    this->appendLog();
    this->lastRuleName = "";
    rulesSinceImprovement ++ ;
    if (this->stack.size < stackSizeToImprove) {
      rulesSinceImprovement = 0;
      stackSizeToImprove = this->stack.size;
    }
    if (rulesSinceImprovement > 10) {
      global.fatal
      << "Too many rule applications without reducing the stack. "
      << global.fatal;
    }
  }
}

bool CodeFormatter::Processor::applyOneRule() {
  CodeFormatter::Element& last = this->stack[this->stack.size - 1];
  CodeFormatter::Element& secondToLast = this->stack[this->stack.size - 2];
  CodeFormatter::Element& thirdToLast = this->stack[this->stack.size - 3];
  CodeFormatter::Element& fourthToLast = this->stack[this->stack.size - 4];
  if (secondToLast.content == "#" && last.content == "include") {
    secondToLast.content += last.content;
    secondToLast.type = CodeFormatter::Element::IncludeLineStart;
    return this->removeLast();
  }
  if (secondToLast.content == "#" && (last.content == "define" || last.content == "ifdef" || last.content == "ifndef" || last.content == "endif") ){
  secondToLast.content += last.content;
    secondToLast.type = CodeFormatter::Element::MacroLineStart;
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::IncludeLineStart &&
    last.type == CodeFormatter::Element::EndLine
  ) {
    secondToLast.type = CodeFormatter::Element::IncludeLine;
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::MacroLineStart &&
    last.type == CodeFormatter::Element::EndLine
  ) {
    secondToLast.type = CodeFormatter::Element::MacroLine;
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::IncludeLineStart &&
    last.type != CodeFormatter::Element::EndLine
  ) {
    secondToLast.content.append(last.content);
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::MacroLineStart &&
    last.type != CodeFormatter::Element::EndLine
  ) {
    secondToLast.content.append(last.content);
    return this->removeLast();
  }
  if (this->owner->isWhiteSpace(last.content)) {
    this->lastRuleName = "trim whitepace";
    return this->removeLast();
  }
  if (
    last.type == CodeFormatter::Element::Unknown &&
    this->owner->isTypeKeyWord(last.content)
  ) {
    this->lastRuleName = "type key word";
    last.type = CodeFormatter::Element::TypeKeyWord;
    return true;
  }
  if (
    last.type == CodeFormatter::Element::Unknown &&
    this->owner->isControlKeyWord(last.content)
  ) {
    this->lastRuleName = "control key word";
    last.type = CodeFormatter::Element::ControlKeyWord;
    return true;
  }
  if (
    last.type == CodeFormatter::Element::Unknown &&
    this->owner->elementTypes.contains(last.content)
  ) {
    this->lastRuleName = "look up element type";
    last.type = this->owner->elementTypes.getValueNoFail(last.content);
    return true;
  }
  if ((
      thirdToLast.type == CodeFormatter::Element::CaseClauseStart ||
      thirdToLast.type == CodeFormatter::Element::CaseClauseMultipleStart
    ) && (secondToLast.isCommandListOrCommand()) && (
      last.type == CodeFormatter::Element::CaseKeyWord ||
      last.type == CodeFormatter::Element::CaseClauseStart ||
      last.type == CodeFormatter::Element::CaseClauseMultipleStart ||
      last.type == CodeFormatter::Element::RightCurlyBrace
    )
  ) {
    thirdToLast.makeFrom2(
      CodeFormatter::Element::CaseClause, thirdToLast, secondToLast
    );
    thirdToLast.makeFrom1(
      CodeFormatter::Element::CaseClauseList, thirdToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::CaseClauseList &&
    secondToLast.type == CodeFormatter::Element::CaseClauseList
  ) {
    thirdToLast.children.addListOnTop(secondToLast.children);
    return this->removeBelowLast(1);
  }
  if (
    last.type == CodeFormatter::Element::Quote ||
    last.type == CodeFormatter::Element::SingleQuote
  ) {
    this->lastRuleName = "quote to expression";
    last.makeFrom1(CodeFormatter::Element::Expression, last);
    return true;
  }
  if (
    secondToLast.type == CodeFormatter::Element::TopLevel &&
    last.isSuitableForTopLevel()
  ) {
    secondToLast.children.addOnTop(last);
    return this->removeLast();
  }
  if (
    secondToLast.isComment() && (
      last.type == CodeFormatter::Element::Comment ||
      last.type == CodeFormatter::Element::CommentMultiline
    )
  ) {
    secondToLast.appendType(
      last, CodeFormatter::Element::CommentCollection
    );
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::Expression && last.isComment()
  ) {
    secondToLast.addChild(last);
    return this->removeLast();
  }
  if (
    secondToLast.isOperatorOrInequality() &&
    last.type == CodeFormatter::Element::InParentheses
  ) {
    this->lastRuleName =
    "in parentheses to expression after expression ending with operator";
    last.makeFrom1(CodeFormatter::Element::Expression, last);
    return true;
  }
  if (
    secondToLast.type == CodeFormatter::Element::InParentheses &&
    last.isOperator()
  ) {
    this->lastRuleName = "in parentheses X to expression X";
    secondToLast.makeFrom1(
      CodeFormatter::Element::Expression, secondToLast
    );
    return true;
  }
  if (
    secondToLast.type == CodeFormatter::Element::ControlKeyWord &&
    last.type == CodeFormatter::Element::InParentheses
  ) {
    this->lastRuleName = "control wants code block";
    secondToLast.makeFrom2(
      CodeFormatter::Element::ControlWantsCodeBlock, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::If &&
    last.type == CodeFormatter::Element::InParentheses
  ) {
    this->lastRuleName = "if wants code block";
    secondToLast.makeFrom2(
      CodeFormatter::Element::IfWantsCodeBlock, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::ControlWantsCodeBlock &&
    last.isCodeBlockOrCommand()
  ) {
    this->lastRuleName = "control with code block";
    secondToLast.makeFrom2(
      CodeFormatter::Element::Command, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::IfWantsCodeBlock &&
    last.isCodeBlockOrCommand()
  ) {
    this->lastRuleName = "if with code block";
    secondToLast.makeFrom2(
      CodeFormatter::Element::IfClause, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    thirdToLast.type == CodeFormatter::Element::IfClause &&
    secondToLast.type == CodeFormatter::Element::Else &&
    last.isCodeBlockOrCommand()
  ) {
    this->lastRuleName = "if else block";
    thirdToLast.addChild(secondToLast);
    thirdToLast.addChild(last);
    return this->removeLast(2);
  }
  if (
    fourthToLast.type == CodeFormatter::Element::IfClause &&
    thirdToLast.type == CodeFormatter::Element::Else &&
    secondToLast.isCodeBlockOrCommand()
  ) {
    this->lastRuleName = "if else block";
    fourthToLast.addChild(thirdToLast);
    fourthToLast.addChild(secondToLast);
    return this->removeBelowLast(2);
  }
  if (
    secondToLast.type == CodeFormatter::Element::IfClause &&
    last.type != CodeFormatter::Element::Else
  ) {
    secondToLast.makeFrom1(CodeFormatter::Element::Command, secondToLast);
    return true;
  }
  if (
    secondToLast.type == CodeFormatter::Element::Colon &&
    last.type == CodeFormatter::Element::Colon
  ) {
    this->lastRuleName = "double-colon";
    secondToLast.type = CodeFormatter::Element::DoubleColon;
    secondToLast.content = "::";
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::FunctionDeclaration &&
    last.type == CodeFormatter::Element::ConstKeyWord
  ) {
    this->lastRuleName = "function declaration const";
    secondToLast.children.addOnTop(last);
    return this->removeLast();
  }
  if (
    last.type == CodeFormatter::Element::Unknown &&
    CodeFormatter::isIdentifierWord(last.content)
  ) {
    this->lastRuleName = "make atom";
    last.type = CodeFormatter::Element::Type::Atom;
    return true;
  }
  if (
    thirdToLast.isIdentifierOrAtom() &&
    secondToLast.type == CodeFormatter::Element::DoubleColon &&
    last.type == CodeFormatter::Element::Atom
  ) {
    this->lastRuleName = "identifier::atom";
    thirdToLast.appendIdentifier(secondToLast);
    thirdToLast.appendIdentifier(last);
    return this->removeLast(2);
  }
  if (
    fourthToLast.type == CodeFormatter::Element::CaseKeyWord &&
    secondToLast.type == CodeFormatter::Element::Colon
  ) {
    this->lastRuleName = "case clause start";
    fourthToLast.makeFrom3(
      CodeFormatter::Element::CaseClauseStart,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(2);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::DefaultKeyWord &&
    secondToLast.type == CodeFormatter::Element::Colon
  ) {
    this->lastRuleName = "default clause";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::CaseClauseStart, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    (secondToLast.type == CodeFormatter::Element::PublicKeyWord ||
  secondToLast.type == CodeFormatter::Element::PrivateKeyWord)&&
    last.type == CodeFormatter::Element::Colon
  ) {
    this->lastRuleName = "visibility clause";
    secondToLast.makeFrom2(
      CodeFormatter::Element::VisibilityClause, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::LeftParenthesis &&
    last.type == CodeFormatter::Element::RightParenthesis
  ) {
    this->lastRuleName = "parentheses match";
    secondToLast.makeFrom2(
      CodeFormatter::Element::InParentheses, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    thirdToLast.isExpressionIdentifierAtomOrFunctionWithArguments() &&
    secondToLast.canBeUnaryOnTheRight() && (
      last.isRightDelimiter() || last.type == CodeFormatter::Element::SemiColon
    )
  ) {
    this->lastRuleName = "expression unary operator )";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::Expression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    secondToLast.type == CodeFormatter::Element::FunctionWithArguments &&
    last.isRightDelimiter()
  ) {
    secondToLast.makeFrom1(
      CodeFormatter::Element::Expression, secondToLast
    );
    return true;
  }
  if (
    secondToLast.type == CodeFormatter::Element::FunctionWithArguments && (
      last.type == CodeFormatter::Element::LessThan ||
      last.type == CodeFormatter::Element::GreaterThan
    )
  ) {
    last.type = CodeFormatter::Element::Operator;
    secondToLast.makeFrom1(
      CodeFormatter::Element::Expression, secondToLast
    );
    return true;
  }
  if (
    thirdToLast.type == CodeFormatter::Element::LeftParenthesis &&
    secondToLast.isSuitableForParenthesesEnclosure() &&
    last.type == CodeFormatter::Element::RightParenthesis
  ) {
    this->lastRuleName = "command list in parentheses";
    thirdToLast.makeFrom3(
      CodeFormatter::Element::InParentheses,
      thirdToLast,
      secondToLast,
      last
    );
    return this->removeLast(2);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::LeftCurlyBrace &&
    secondToLast.type == CodeFormatter::Element::CommaList &&
    last.type == CodeFormatter::Element::RightCurlyBrace
  ) {
    this->lastRuleName = "comma list in curly braces";
    thirdToLast.makeFrom3(
      CodeFormatter::Element::CurlyBraceCommaDelimitedList,
      thirdToLast,
      secondToLast,
      last
    );
    thirdToLast.makeFrom1(CodeFormatter::Element::Expression, thirdToLast);
    return this->removeLast(2);
  }
  if (
    fourthToLast.isExpressionIdentifierOrAtom() &&
    thirdToLast.type == CodeFormatter::Element::LeftBracket &&
    secondToLast.isExpressionIdentifierOrAtom() &&
    last.type == CodeFormatter::Element::RightBracket
  ) {
    this->lastRuleName = "[expression]";
    CodeFormatter::Element element;
    element.makeFrom3(
      CodeFormatter::Element::InBrackets,
      thirdToLast,
      secondToLast,
      last
    );
    fourthToLast.makeFrom2(
      CodeFormatter::Element::Expression, fourthToLast, element
    );
    return this->removeLast(3);
  }
  if (
    secondToLast.type == CodeFormatter::Element::LeftCurlyBrace &&
    last.type == CodeFormatter::Element::RightCurlyBrace
  ) {
    this->lastRuleName = "{}";
    secondToLast.makeFrom2(
      CodeFormatter::Element::CodeBlock, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    last.type == CodeFormatter::Element::Unknown &&
    this->owner->isOperator(last.content)
  ) {
    this->lastRuleName = "combine";
    last.type = CodeFormatter::Element::Operator;
    return true;
  }
  if (
    secondToLast.content != "" &&
    last.content != "" &&
    this->owner->isOperator(secondToLast.content + last.content)
  ) {
    this->lastRuleName = "add operators";
    secondToLast.content = secondToLast.content + last.content;
    secondToLast.type = CodeFormatter::Element::Operator;
    return this->removeLast();
  }
  if (
    thirdToLast.isOperator() &&
    secondToLast.type == CodeFormatter::Element::Atom && (
      last.type == CodeFormatter::Element::Comma || last.isRightDelimiter()
    )
  ) {
    this->lastRuleName = "operator atom ?";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::Expression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    secondToLast.type == CodeFormatter::Element::FunctionWithArguments &&
    last.type == CodeFormatter::Element::Operator
  ) {
    this->lastRuleName = "function with arguments to expression";
    secondToLast.makeFrom1(
      CodeFormatter::Element::Expression, secondToLast
    );
    return true;
  }
  if (
    thirdToLast.type == CodeFormatter::Element::ConstKeyWord &&
    secondToLast.isExpressionIdentifierOrAtom() &&
    last.type != CodeFormatter::Element::Colon &&
    last.type != CodeFormatter::Element::DoubleColon
  ) {
    this->lastRuleName = "const something";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    fourthToLast.isSuitableForTypeExpression() &&
    thirdToLast.type == CodeFormatter::Element::LessThan &&
    secondToLast.isSuitableForTemplateArgument() &&
    last.type == CodeFormatter::Element::GreaterThan
  ) {
    this->lastRuleName =
    "type expression less than type expression greater than";
    fourthToLast.makeFrom4(
      CodeFormatter::Element::TypeExpression,
      fourthToLast,
      thirdToLast,
      secondToLast,
      last
    );
    return this->removeLast(3);
  }
  if (
    secondToLast.type == CodeFormatter::Element::TypeExpression &&
    last.isStarOrAmpersand()
  ) {
    this->lastRuleName = "absorb star or ampersand";
    secondToLast.children.addOnTop(last);
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::Ampersand &&
    last.type == CodeFormatter::Element::Ampersand
  ) {
    this->lastRuleName = "ampersand and ampersand";
    secondToLast.clear();
    secondToLast.content = "&&";
    secondToLast.type = CodeFormatter::Element::Operator;
    return this->removeLast();
  }
  if (secondToLast.isTypeWord() && last.isTypeWord()) {
    this->lastRuleName = "type and type to type expression";
    secondToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    thirdToLast.isSuitableForTypeExpression() &&
    secondToLast.isIdentifierOrAtom() &&
    !last.isColonOrDoubleColon()
  ) {
    this->lastRuleName = "type and identifier";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeAndIdentifier,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.isSuitableForTypeExpression() &&
    secondToLast.type == CodeFormatter::Element::CommaList &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    this->lastRuleName = "type comma_list semicolon";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeAndIdentifier,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    secondToLast.isTypeOrIdentifierOrExpression() && last.isParenthesesBlock()
  ) {
    this->lastRuleName = "identifier(arguments)";
    secondToLast.makeFrom2(
      CodeFormatter::Element::FunctionWithArguments, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    this->isSuitableForExpressionOperatorExpression(
      fourthToLast, thirdToLast, secondToLast, last
    )
  ) {
    this->lastRuleName = "expression operator expression";
    // Operators such as < and > may not have their type
    // set to Operator.
    thirdToLast.type = CodeFormatter::Element::Operator;
    fourthToLast.makeFrom3(
      CodeFormatter::Element::Expression,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(2);
  }
  if (
    this->isSuitableForUnaryOperatorExpression(
      fourthToLast, thirdToLast, secondToLast, last
    )
  ) {
    this->lastRuleName = "X unary operator expression Y";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::Expression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    fourthToLast.isTypeWordOrTypeExpression() &&
    thirdToLast.isExpressionIdentifierOrAtom() &&
    secondToLast.type == CodeFormatter::Element::Colon &&
    last.type != CodeFormatter::Element::Colon
  ) {
    this->lastRuleName = "type expression : expression";
    fourthToLast.makeFrom2(
      CodeFormatter::Element::Expression, fourthToLast, thirdToLast
    );
    fourthToLast.type = CodeFormatter::Element::Expression;
    this->stack.removeIndexShiftDown(this->stack.size - 3);
    return true;
  }
  if (
    fourthToLast.isExpressionOrAtom() &&
    thirdToLast.type == CodeFormatter::Element::Colon &&
    secondToLast.isExpressionOrAtom() &&
    last.type == CodeFormatter::Element::RightParenthesis
  ) {
    this->lastRuleName = "expression : expression)";
    thirdToLast.type = CodeFormatter::Element::Operator;
    fourthToLast.makeFrom3(
      CodeFormatter::Element::Expression,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(2);
  }
  // fourthToLast.type == CodeFormatter::Element::ControlKeyWord &&
  bool isInIfClause = fourthToLast.type == CodeFormatter::Element::If &&
  thirdToLast.type == CodeFormatter::Element::LeftParenthesis;
  if (
    !isInIfClause &&
    !thirdToLast.isOperator() &&
    thirdToLast.type != CodeFormatter::Element::LessThan &&
    thirdToLast.type != CodeFormatter::Element::GreaterThan &&
    secondToLast.isIdentifierOrAtom() && (
      last.type == CodeFormatter::Element::Ampersand ||
      last.type == CodeFormatter::Element::Star
    )
  ) {
    this->lastRuleName = "type and ampersand or star";
    secondToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    thirdToLast.isExpressionIdentifierOrAtom() &&
    secondToLast.isExpressionIdentifierOrAtom() &&
    !last.isColonOrDoubleColon() &&
    last.type != CodeFormatter::Element::LessThan
  ) {
    this->lastRuleName = "expression and atom";
    thirdToLast.appendExpression(secondToLast);
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.isParenthesesBlock() && secondToLast.isExpressionOrAtom()
  ) {
    this->lastRuleName = "in parentheses and expression";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::Expression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::CommaList &&
    secondToLast.isSuitableForCommaListElement() &&
    last.type == CodeFormatter::Element::Comma
  ) {
    this->lastRuleName = "comma list and element";
    thirdToLast.children.addOnTop(secondToLast);
    thirdToLast.children.addOnTop(last);
    return this->removeLast(2);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::CommaList &&
    secondToLast.isSuitableForCommaListElement() && (
      last.isRightDelimiter() ||
      last.type == CodeFormatter::Element::SemiColon ||
      last.type == CodeFormatter::Element::GreaterThan
    )
  ) {
    this->lastRuleName = "comma list and delimiter";
    thirdToLast.children.addOnTop(secondToLast);
    return this->removeBelowLast(1);
  }
  if (
    fourthToLast.type == CodeFormatter::Element::TypeAndIdentifier &&
    thirdToLast.type == CodeFormatter::Element::Comma &&
    secondToLast.type == CodeFormatter::Element::Atom && (
      last.isRightDelimiter() ||
      last.type == CodeFormatter::Element::Comma ||
      last.type == CodeFormatter::Element::SemiColon
    )
  ) {
    this->lastRuleName = "type identifier comma atom delimiter";
    fourthToLast.addChild(thirdToLast);
    fourthToLast.addChild(secondToLast);
    return this->removeBelowLast(2);
  }
  if (
    fourthToLast.type == CodeFormatter::Element::TypeAndIdentifier &&
    thirdToLast.type == CodeFormatter::Element::Comma &&
    secondToLast.type == CodeFormatter::Element::TypeAndIdentifier &&
    last.type == CodeFormatter::Element::Comma
  ) {
    this->lastRuleName = "type identifier comma type identifier comma";
    fourthToLast.makeFrom4(
      CodeFormatter::Element::CommaList,
      fourthToLast,
      thirdToLast,
      secondToLast,
      last
    );
    return this->removeLast(3);
  }
  if (
    fourthToLast.type == CodeFormatter::Element::TypeAndIdentifier &&
    thirdToLast.type == CodeFormatter::Element::Comma &&
    secondToLast.type == CodeFormatter::Element::TypeAndIdentifier && (
      last.isRightDelimiter() || last.type == CodeFormatter::Element::SemiColon
    )
  ) {
    this->lastRuleName = "type identifier comma type identifier delimiter";
    fourthToLast.makeFrom3(
      CodeFormatter::Element::CommaList,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(2);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::Return && (
      secondToLast.type == CodeFormatter::Element::Expression ||
      secondToLast.type == CodeFormatter::Element::FunctionWithArguments ||
      secondToLast.type == CodeFormatter::Element::TypeAndIdentifier
    ) &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    this->lastRuleName = "return expression semicolon";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::ReturnedExpression,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(1);
  }
  if ((
      secondToLast.isIdentifierOrAtom() ||
      secondToLast.type == CodeFormatter::Element::Return ||
      secondToLast.type == CodeFormatter::Element::FunctionWithArguments ||
      secondToLast.type == CodeFormatter::Element::ReturnedExpression
    ) &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    this->lastRuleName = "identifier and semicolon";
    secondToLast.makeFrom1(
      CodeFormatter::Element::Expression, secondToLast
    );
    return true;
  }
  if (
    fourthToLast.isExpressionIdentifierOrAtom() && (
      thirdToLast.type == CodeFormatter::Element::LessThan ||
      thirdToLast.type == CodeFormatter::Element::GreaterThan
    ) &&
    secondToLast.isExpressionIdentifierOrAtom() && (
      last.type == CodeFormatter::Element::SemiColon ||
      last.isRightDelimiter() || (
        last.isOperator() &&
        !this->owner->rightOperatorOverridesLeft(thirdToLast, last)
      )
    )
  ) {
    this->lastRuleName = "expression lessThan expression";
    thirdToLast.type = CodeFormatter::Element::Operator;
    fourthToLast.makeFrom3(
      CodeFormatter::Element::Expression,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(2);
  }
  if (
    !thirdToLast.isOperator() &&
    secondToLast.isSuitableForCommand() &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    this->lastRuleName = "expression and semicolon";
    secondToLast.makeFrom2(
      CodeFormatter::Element::Command, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    secondToLast.isExpressionIdentifierOrAtom() &&
    last.type == CodeFormatter::Element::Comma
  ) {
    this->lastRuleName = "expression, ";
    secondToLast.makeFrom2(
      CodeFormatter::Element::CommaList, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    thirdToLast.isCommandListOrCommand() &&
    secondToLast.type == CodeFormatter::Element::Command
  ) {
    this->lastRuleName = "commandList command something";
    thirdToLast.appendCommand(secondToLast);
    return this->removeBelowLast(1);
  }
  if (
    secondToLast.isCommandListOrCommand() && (
      last.type == CodeFormatter::Element::Command ||
      last.type == CodeFormatter::Element::Comment ||
      last.type == CodeFormatter::Element::CommentMultiline
    )
  ) {
    this->lastRuleName = "commandList command";
    secondToLast.appendCommand(last);
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::Command && (
      last.type == CodeFormatter::Element::Comment ||
      last.type == CodeFormatter::Element::CommentMultiline ||
      last.type == CodeFormatter::Element::CommentCollection
    )
  ) {
    this->lastRuleName = "command comment";
    secondToLast.appendCommand(last);
    return this->removeLast();
  }
  if ((
      secondToLast.type == CodeFormatter::Element::Comment ||
      secondToLast.type == CodeFormatter::Element::CommentMultiline ||
      secondToLast.type == CodeFormatter::Element::CommentCollection
    ) &&
    last.type == CodeFormatter::Element::Command
  ) {
    this->lastRuleName = "command comment";
    secondToLast.appendCommand(last);
    return this->removeLast();
  }
  if (
    thirdToLast.isCommandListOrCommand() && (secondToLast.isExpressionIdentifierOrAtom()) &&
    last.type == CodeFormatter::Element::RightParenthesis
  ) {
    this->lastRuleName = "commandList expression)";
    thirdToLast.appendCommand(secondToLast);
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::LeftCurlyBrace && (
      secondToLast.isCommandListOrCommand() ||
      secondToLast.type == CodeFormatter::Element::CaseClauseList
    ) &&
    last.type == CodeFormatter::Element::RightCurlyBrace
  ) {
    thirdToLast.makeFrom3(
      CodeFormatter::Element::CodeBlock, thirdToLast, secondToLast, last
    );
    return this->removeLast(2);
  }
  if (
    secondToLast.type == CodeFormatter::Element::TypeExpression &&
    last.isParenthesesBlock()
  ) {
    this->lastRuleName = "typeExpression(arguments)";
    secondToLast.makeFrom2(
      CodeFormatter::Element::FunctionWithArguments, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::TypeAndIdentifier &&
    last.isParenthesesBlock()
  ) {
    this->lastRuleName = "type_identifier + parentheses block";
    secondToLast.makeFrom2(
      CodeFormatter::Element::FunctionDeclaration, secondToLast, last
    );
    return this->removeLast();
  }
  if ((
      secondToLast.type == CodeFormatter::Element::FunctionDeclaration ||
      secondToLast.type == CodeFormatter::Element::FunctionWithArguments
    ) &&
    last.isCodeBlock()
  ) {
    this->lastRuleName = "function declaration + code block";
    secondToLast.makeFrom2(
      CodeFormatter::Element::FunctionDefinition, secondToLast, last
    );
    secondToLast.makeFrom1(CodeFormatter::Element::Command, secondToLast);
    return this->removeLast();
  }
  if (
    thirdToLast.isIdentifierOrAtom() &&
    secondToLast.isParenthesesBlock() &&
    last.isCodeBlock()
  ) {
    this->lastRuleName = "atom(...){...}";
    thirdToLast.makeFrom3(
      CodeFormatter::Element::ConstructorExternal,
      thirdToLast,
      secondToLast,
      last
    );
    return this->removeLast(2);
  }
  if (
    thirdToLast.isParenthesesBlock() &&
    secondToLast.type == CodeFormatter::Element::Expression
  ) {
    this->lastRuleName = "in parentheses and expression";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::Expression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  return false;
}

bool CodeFormatter::Processor::isSuitableForUnaryOperatorExpression(
  CodeFormatter::Element& first,
  CodeFormatter::Element& unary,
  CodeFormatter::Element& expression,
  Element& lookAhead
) {
  if (!expression.isExpressionIdentifierAtomOrFunctionWithArguments()) {
    return false;
  }
  if (!unary.canBeUnaryOnTheLeft()) {
    return false;
  }
  if (
    lookAhead.type == CodeFormatter::Element::Colon ||
    lookAhead.type == CodeFormatter::Element::DoubleColon
  ) {
    return false;
  }
  if (lookAhead.isOperator()) {
    return false;
  }
  if (first.isIdentifierOrAtom()) {
    return false;
  }
  return true;
}

bool CodeFormatter::Processor::isSuitableForExpressionOperatorExpression(
  CodeFormatter::Element& left,
  CodeFormatter::Element& middle,
  CodeFormatter::Element& right,
  CodeFormatter::Element& lookAhead
) {
  if (
    !left.isExpressionIdentifierAtomOrFunctionWithArguments() &&
    left.type != CodeFormatter::Element::TypeAndIdentifier
  ) {
    return false;
  }
  if (!right.isExpressionIdentifierAtomOrFunctionWithArguments()) {
    return false;
  }
  if (!middle.isOperatorOrInequality()) {
    return false;
  }
  if (
    lookAhead.isColonOrDoubleColon() ||
    lookAhead.content == "-" ||
    lookAhead.type == CodeFormatter::Element::LeftParenthesis ||
    lookAhead.type == CodeFormatter::Element::LeftBracket
  ) {
    return false;
  }
  if (this->owner->rightOperatorOverridesLeft(middle, lookAhead)) {
    return false;
  }
  if (
    middle.type == CodeFormatter::Element::LessThan && (
      lookAhead.type == CodeFormatter::Element::Comma ||
      lookAhead.type == CodeFormatter::Element::GreaterThan
    )
  ) {
    // This could be something of the form:
    // List<a,
    // which can extend to a template: List<a,b>;
    // or of the form:
    // List<a>.
    return false;
  }
  return true;
}

void CodeFormatter::Processor::appendLog() {
  if (!this->flagPrepareReport) {
    return;
  }
  if (this->debugLog.size() > 1000000) {
    if (!this->flagExceededReportSize) {
      this->debugLog += " ... log is too large.";
      this->flagExceededReportSize = true;
    }
    return;
  }
  this->debugLog += this->toStringStack() +
  "<b style='color:red'>[[" +
  this->lastRuleName +
  "]]</b><br>";
}

std::string CodeFormatter::Processor::toStringStack() {
  std::stringstream out;
  for (
    int i = this->owner->dummyElements; i < this->stack.size; i ++
  ) {
    out
    << "<b style='color:brown'>"
    << HtmlRoutines::convertStringToHtmlString(
      this->stack[i].toStringWithoutType(), false
    )
    << "</b>["
    << this->stack[i].toStringType(this->stack[i].type)
    << "] ";
  }
  return out.str();
}

bool CodeFormatter::Processor::removeLast() {
  return this->removeLast(1);
}

bool CodeFormatter::Processor::removeLast(int count) {
  for (int i = 0; i < count; i ++) {
    this->stack.removeLastObject();
  }
  return true;
}

bool CodeFormatter::Processor::removeBelowLast(int count) {
  for (int i = 0; i < count; i ++) {
    this->stack.removeIndexShiftDown(this->stack.size - 2);
  }
  return true;
}

bool CodeFormatter::shouldSeparateWithSpace(
  const std::string& left, const std::string& right
) {
  if (left == "") {
    return false;
  }
  List<std::string> pair;
  pair.addOnTop(left);
  pair.addOnTop(right);
  if (this->pairsNotSeparatedBySpace.contains(pair)) {
    return false;
  }
  if (this->doesntNeedSpaceToTheRightContainer.contains(left)) {
    return false;
  }
  if (this->doesntNeedSpaceToTheLeftContainer.contains(right)) {
    return false;
  }
  return true;
}

bool CodeFormatter::isTypeKeyWord(const std::string& input) {
  return this->typeKeyWords.contains(input);
}

bool CodeFormatter::isControlKeyWord(const std::string& input) {
  return this->controlKeyWords.contains(input);
}

bool CodeFormatter::isWhiteSpace(const std::string& input) {
  return input == "\n" || input == "\t" || input == " " || input == "\r";
}

bool CodeFormatter::writeFormatedCode(std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::writeFormatedCode");
  std::fstream fileOut;
  if (
    !FileOperations::openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(
      fileOut, this->outputFileName, false, true, false
    )
  ) {
    if (comments != nullptr) {
      *comments << "Failed to open source code formatting output file. ";
    }
    return false;
  }
  fileOut << this->transformedContent;
  return true;
}

bool CodeFormatter::rightOperatorOverridesLeft(
  const CodeFormatter::Element& leftOperator,
  const CodeFormatter::Element& rightOperator
) {
  if (!rightOperator.isOperatorOrInequality()) {
    return false;
  }
  if (!this->operatorOverrides.contains(rightOperator.content)) {
    return false;
  }
  return
  this->operatorOverrides.getValueNoFail(rightOperator.content).contains(
    leftOperator.content
  );
}

void CodeFormatter::wirePointersRecursively(
  CodeFormatter::Element& current, Element* parent, int indexInParent
) {
  current.initializePointers(this, parent, indexInParent);
  for (int i = 0; i < current.children.size; i ++) {
    this->wirePointersRecursively(current.children[i], &current, i);
  }
}

void CodeFormatter::correctMultiArguments(
  CodeFormatter::Element& inputOutput
) {
  if (
    inputOutput.type != CodeFormatter::Element::Expression ||
    inputOutput.children.size != 3
  ) {
    return;
  }
  CodeFormatter::Element& middle = inputOutput.children[1];
  if (!this->isOperatorSuitableForNormalization(middle)) {
    return;
  }
  List<CodeFormatter::Element> collector;
  this->collectMultiArguments(middle.content, inputOutput, collector);
  if (collector.size < 3) {
    return;
  }
  inputOutput.children.clear();
  for (int i = 0; i < collector.size; i ++) {
    inputOutput.addChild(collector[i]);
  }
}

void CodeFormatter::collectMultiArguments(
  const std::string& operatorName,
  CodeFormatter::Element& current,
  List<Element>& output
) {
  if (
    current.type != CodeFormatter::Element::Expression ||
    current.children.size != 3
  ) {
    output.addOnTop(current);
    return;
  }
  CodeFormatter::Element& middle = current.children[1];
  if (!middle.isOperator() || middle.content != operatorName) {
    output.addOnTop(current);
    return;
  }
  this->collectMultiArguments(operatorName, current.children[0], output);
  output.addOnTop(middle);
  this->collectMultiArguments(operatorName, current.children[2], output);
}

void CodeFormatter::normalizeBinaryOperationsRecursively(
  CodeFormatter::Element& current
) {
  this->correctMultiArguments(current);
  for (CodeFormatter::Element& child : current.children) {
    this->normalizeBinaryOperationsRecursively(child);
  }
}

bool CodeFormatter::isOperatorSuitableForNormalization(
  const CodeFormatter::Element& element
) {
  if (!element.isOperator()) {
    return false;
  }
  const std::string& name = element.content;
  return
  name == "||" ||
  name == "&&" ||
  name == "+" ||
  name == "-" ||
  name == "<<";
}

