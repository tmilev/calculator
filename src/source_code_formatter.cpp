#include "general_file_operations_encodings.h"
#include "general_logging_global_variables.h"
#include "general_strings.h"
#include "html_routines.h"
#include "math_basics.h"
#include "source_code_formatter.h"

CodeFormatter::Element::Element() {
  this->indentationLevel = 0;
  this->newLinesAfter = 0;
  this->whiteSpaceBefore = 0;
  this->columnFinal = 0;
  this->type = CodeFormatter::Element::Unknown;
  this->owner = nullptr;
  this->parent = nullptr;
  this->indexInParent = - 1;
  this->maximumDesiredLineLengthOverride = - 1;
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

std::string CodeFormatter::Element::toStringMyType() const {
  return this->toStringType(this->type);
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
  case CodeFormatter::Element::InAngleBrackets:
    return "InAngleBrackets";
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
  case CodeFormatter::Element::NewKeyWord:
    return "new";
  case CodeFormatter::Element::DeleteKeyWord:
    return "delete";
  case CodeFormatter::Element::Operator:
    return "operator";
  case CodeFormatter::Element::Quote:
    return "quote";
  case CodeFormatter::Element::OperatorKeyWord:
    return "operatorKeyWord";
  case CodeFormatter::Element::ConstKeyWord:
    return "const";
  case CodeFormatter::Element::TypeAdjectiveKeyWord:
    return "typeAdjective";
  case CodeFormatter::Element::FriendKeyWord:
    return "friend";
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
  case CodeFormatter::Element::FunctionWithArgumentsAndInitializerList:
    return "functionWithArgumentsAndInitializerList";
  case CodeFormatter::Element::TypeAndIdentifier:
    return "typeAndIdentifier";
  case CodeFormatter::Element::TypeExpression:
    return "typeExpression";
  case CodeFormatter::Element::TryKeyWord:
    return "try";
  case CodeFormatter::Element::CatchKeyWord:
    return "catch";
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
  case CodeFormatter::Element::VisibilityKeyWord:
    return "visibility";
  case CodeFormatter::Element::UnionKeyWord:
    return "union";
  case CodeFormatter::Element::ClassKeyWord:
    return "class";
  case CodeFormatter::Element::StructKeyWord:
    return "struct";
  case CodeFormatter::Element::TypenameKeyWord:
    return "typename";
  case CodeFormatter::Element::TemplateKeyWord:
    return "template";
  case CodeFormatter::Element::ClassDeclaration:
    return "classDeclaration";
  case CodeFormatter::Element::ClassDefinition:
    return "classDefinition";
  case CodeFormatter::Element::TemplateClause:
    return "templateClause";
  case CodeFormatter::Element::EnumKeyWord:
    return "enum";
  case CodeFormatter::Element::EnumDeclaration:
    return "enumDeclaration";
  case CodeFormatter::Element::EnumDefinition:
    return "enumDefinition";
  case CodeFormatter::Element::VisibilityClause:
    return "visibilityClause";
  case CodeFormatter::Element::DefaultKeyWord:
    return "default";
  case CodeFormatter::Element::ConstructorExternal:
    return "constructorExternal";
  case CodeFormatter::Element::Exclamation:
    return "!";
  case CodeFormatter::Element::Tilde:
    return "tilde";
  case CodeFormatter::Element::QuestionMark:
    return "?";
  case CodeFormatter::Element::If:
    return "if";
  case CodeFormatter::Element::DoKeyWord:
    return "do";
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
  case CodeFormatter::Element::EndFile:
    return "EndFile";
  }
  return "????";
}

void CodeFormatter::Element::appendExpression(
  const CodeFormatter::Element& other
) {
  this->appendType(other, CodeFormatter::Element::Expression);
}

void CodeFormatter::Element::appendType(
  const CodeFormatter::Element& other, CodeFormatter::Element::Type inputType
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
  if (this == &first || this == &second || this == &third || this == &fourth) {
    CodeFormatter::Element firstCopy = first;
    CodeFormatter::Element secondCopy = second;
    CodeFormatter::Element thirdCopy = third;
    CodeFormatter::Element fourthCopy = fourth;
    this->makeFrom4(inputType, firstCopy, secondCopy, thirdCopy, fourthCopy);
    return;
  }
  this->clear();
  this->type = inputType;
  this->addChild(first);
  this->addChild(second);
  this->addChild(third);
  this->addChild(fourth);
}

void CodeFormatter::Element::makeFrom5(
  CodeFormatter::Element::Type inputType,
  const Element& leftmost,
  const Element& secondToLeft,
  const Element& middle,
  const Element& secondToRight,
  const Element& right
) {
  if (
    this == &leftmost ||
    this == &secondToLeft ||
    this == &middle ||
    this == &secondToRight ||
    this == &right
  ) {
    CodeFormatter::Element firstCopy = leftmost;
    CodeFormatter::Element secondCopy = secondToLeft;
    CodeFormatter::Element thirdCopy = middle;
    CodeFormatter::Element fourthCopy = secondToRight;
    CodeFormatter::Element fifthCopy = right;
    this->makeFrom5(
      inputType, firstCopy, secondCopy, thirdCopy, fourthCopy, fifthCopy
    );
    return;
  }
  this->clear();
  this->type = inputType;
  this->addChild(leftmost);
  this->addChild(secondToLeft);
  this->addChild(middle);
  this->addChild(secondToRight);
  this->addChild(right);
}

void CodeFormatter::Element::addChild(const CodeFormatter::Element& other) {
  this->children.addOnTop(other);
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
  this->type == CodeFormatter::Element::TypeAndIdentifier ||
  this->type == CodeFormatter::Element::FunctionWithArguments ||
  this->type == CodeFormatter::Element::Expression ||
  this->type == CodeFormatter::Element::CommaList ||
  this->type == CodeFormatter::Element::ClassDeclaration;
}

bool CodeFormatter::Element::isSuitableForParenthesesEnclosure() const {
  return
  this->isCommandListOrCommand() ||
  this->isExpressionIdentifierOrAtom() ||
  this->type == CodeFormatter::Element::InParentheses ||
  this->type == CodeFormatter::Element::TypeAndIdentifier ||
  this->type == CodeFormatter::Element::Star ||
  this->type == CodeFormatter::Element::TypeKeyWord ||
  this->type == CodeFormatter::Element::TypeExpression ||
  this->type == CodeFormatter::Element::FunctionDeclaration ||
  this->type == CodeFormatter::Element::CommaList;
}

bool CodeFormatter::Element::isRightDelimiter() const {
  return
  this->type == CodeFormatter::Element::RightBracket ||
  this->type == CodeFormatter::Element::RightCurlyBrace ||
  this->type == CodeFormatter::Element::RightParenthesis;
}

bool CodeFormatter::Element::isLeftDelimiter() const {
  return
  this->type == CodeFormatter::Element::LeftBracket ||
  this->type == CodeFormatter::Element::LeftParenthesis ||
  this->type == CodeFormatter::Element::LeftCurlyBrace;
}

bool CodeFormatter::Element::isRightDelimiterOrSemicolon() const {
  return
  this->isRightDelimiter() ||
  this->type == CodeFormatter::Element::SemiColon;
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

bool CodeFormatter::Element::
isExpressionIdentifierAtomFunctionWithArgumentsOrInParentheses() const {
  return
  this->isExpressionIdentifierAtomOrFunctionWithArguments() ||
  this->type == CodeFormatter::Element::InParentheses;
}

bool CodeFormatter::Element::isSuitableForTopLevel() const {
  return
  this->type == CodeFormatter::Element::IncludeLine ||
  this->type == CodeFormatter::Element::MacroLine ||
  this->type == CodeFormatter::Element::Command ||
  this->type == CodeFormatter::Element::Comment ||
  this->type == CodeFormatter::Element::CommentMultiline ||
  this->type == CodeFormatter::Element::ClassDefinition ||
  this->type == CodeFormatter::Element::EnumDefinition;
}

bool CodeFormatter::Element::isSuitableForCommand() const {
  return
  this->type == CodeFormatter::Element::Expression ||
  this->type == CodeFormatter::Element::FunctionWithArguments ||
  this->type == CodeFormatter::Element::CommaList ||
  this->type == CodeFormatter::Element::InParentheses ||
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
  this->content == "~" ||
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

std::string CodeFormatter::Element::formatDefaultToString() {
  std::stringstream out;
  this->formatDefault(out);
  return out.str();
}

void CodeFormatter::Element::formatDefault(std::stringstream& out) {
  if (this->type == CodeFormatter::Element::Type::Dummy) {
    return;
  }
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

bool CodeFormatter::Element::computeIndentationLessThan() {
  if (this->parent->type != CodeFormatter::Element::TypeExpression) {
    this->computeIndentationAtomic();
    return true;
  }
  return true;
}

bool CodeFormatter::Element::computeIndentationGreaterThan() {
  if (this->parent->type != CodeFormatter::Element::TypeExpression) {
    this->computeIndentationAtomic();
    return true;
  }
  CodeFormatter::Element * previous = previousAtom();
  if (previous == nullptr) {
    return true;
  }
  if (previous->type == CodeFormatter::Element::GreaterThan) {
    this->computeIndentationAtomic();
    return true;
  }
  return true;
}

bool CodeFormatter::Element::computeIndentationTemplateClause() {
  if (this->children.size >= 2) {
    if (this->children[1].type == CodeFormatter::Element::InAngleBrackets) {
      this->children[1].leftMostAtomUnderMe()->whiteSpaceBefore = 1;
    }
  }
  this->computeIndentationBasic(0);
  this->rightMostAtomUnderMe()->newLinesAfter = 1;
  return true;
}

bool CodeFormatter::Element::computeIndentationMacroline() {
  if (this->previousAtom()->newLinesAfter == 0) {
    this->previousAtom()->newLinesAfter = 1;
  }
  if (this->rightMostAtomUnderMe()->newLinesAfter == 0) {
    this->rightMostAtomUnderMe()->newLinesAfter = 1;
  }
  return this->computeIndentationBasic(0);
}

int CodeFormatter::Element::maximumLineLength() {
  if (this->maximumDesiredLineLengthOverride <= 0) {
    return this->owner->maximumDesiredLineLength;
  }
  return this->maximumDesiredLineLengthOverride;
}

void CodeFormatter::Element::computeIndentationAtomic() {
  CodeFormatter::Element* previous = this->previousAtom();
  if (previous != nullptr) {
    this->owner->applyNewLineExceptions(*previous, *this);
  }
  if (previous == nullptr) {
    this->whiteSpaceBefore = 0;
  } else if (this->owner->mustSplitWithWhitespace(*previous, *this)) {
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
  if (this->columnFinal < this->maximumLineLength()) {
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
    int result = this->content.size();
    if (this->whiteSpaceBefore) {
      result ++;
    }
    return result;
  }
  int result = 0;
  for (int i = 0; i < this->children.size; i ++) {
    result += this->children[i].minimalSizeWithSpacebars();
  }
  return result;
}

bool CodeFormatter::Element::computeIndentationInBrackets() {
  this->computeIndentationInParentheses();
  return true;
}

bool CodeFormatter::Element::computeIndentationAngleBrackets() {
  this->computeIndentationInParentheses();
  return true;
}

bool CodeFormatter::Element::computeIndentationInParentheses(
  bool forceBreakLines
) {
  if (this->children.size != 3) {
    this->computeIndentationBasic(0);
    return true;
  }
  this->children[0].indentationLevel = this->indentationLevel;
  this->children[2].indentationLevel = this->indentationLevel;
  this->children[0].computeIndentation();
  int middleSize = this->children[1].minimalSizeWithSpacebars();
  this->children[1].indentationLevel =
  this->indentationLevel + this->owner->tabLength;
  // +4 allows for open parentheses, space, open curly brace.
  bool tooWide =
  this->children[0].rightMostAtomUnderMe()->columnFinal + middleSize + 4 >
  this->maximumLineLength();
  this->children[1].computeIndentation();
  bool middleHasNewLines =
  this->children[1].containsNewLineAfterExcludingComments();
  bool mustBreakLines = tooWide || middleHasNewLines || forceBreakLines;
  if (mustBreakLines) {
    this->children[0].rightMostAtomUnderMe()->newLinesAfter = 1;
    this->children[1].resetWhitespaceRecursively();
    this->children[1].indentationLevel =
    this->indentationLevel + this->owner->tabLength;
    this->children[1].computeIndentation();
    this->children[1].rightMostAtomUnderMe()->newLinesAfter = 1;
  }
  this->children[2].computeIndentation();
  if (
    !mustBreakLines && (
      this->children[0].containsNewLineAfterExcludingComments() ||
      this->children[1].containsNewLineAfterExcludingComments()
    )
  ) {
    return this->computeIndentationInParentheses(true);
  }
  return true;
}

bool CodeFormatter::Element::computeIndentationOperator() {
  this->computeIndentationAtomic();
  return true;
}

bool CodeFormatter::Element::computeIndentationTypeExpression() {
  this->computeIndentationBasic(0);
  return true;
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

bool CodeFormatter::Element::computeIndentationReturnedExpression() {
  this->computeIndentationBasic(0);
  bool shouldBreak = this->containsNewLineAfterExcludingComments();
  if (
    !shouldBreak &&
    this->rightMostAtomUnderMe()->columnFinal + 1 >= this->maximumLineLength()
  ) {
    shouldBreak = true;
  }
  if (!shouldBreak) {
    return true;
  }
  if (this->children.size != 2) {
    return true;
  }
  CodeFormatter::Element& returned = this->children[0];
  CodeFormatter::Element& expression = this->children[1];
  returned.resetWhitespaceRecursively();
  returned.computeIndentation();
  returned.newLinesAfter = 1;
  expression.breakExpression();
  return true;
}

bool CodeFormatter::Element::computeIndentation() {
  STACK_TRACE("CodeFormatter::Element::computeIndentation");
  if (this->parent == nullptr) {
    // This must be the top level element.
    for (int i = 0; i < this->children.size; i ++) {
      this->children[i].computeIndentation();
    }
    return true;
  }
  if (this->previousAtom() == nullptr) {
    // The first element of code should be a dummy element, no need to
    // compute its indentation.
    return true;
  }
  if (this->maximumDesiredLineLengthOverride > 0) {
    for (CodeFormatter::Element& element : this->children) {
      element.maximumDesiredLineLengthOverride =
      this->maximumDesiredLineLengthOverride;
    }
  }
  switch (this->type) {
  case CodeFormatter::Element::TopLevel:
    return this->computeIndentationTopLevel();
  case CodeFormatter::Element::CodeBlock:
    return this->computeIndentationCodeBlock();
  case CodeFormatter::Element::CurlyBraceCommaDelimitedList:
    return this->computeIndentationCurlyBraceCommaDelimitedList();
  case CodeFormatter::Element::ControlWantsCodeBlock:
    return this->computeIndentationControlWantsCodeBlock();
  case CodeFormatter::Element::CaseClause:
    return this->computeIndentationCaseClause();
  case CodeFormatter::Element::CaseClauseStart:
    return this->computeIndentationCaseClauseStart();
  case CodeFormatter::Element::CaseClauseMultipleStart:
    return this->computeIndentationCaseClauseMultipleStart();
  case CodeFormatter::Element::CaseClauseList:
    return this->computeIndentationCaseClauseList();
  case CodeFormatter::Element::ReturnedExpression:
    return this->computeIndentationReturnedExpression();
  case CodeFormatter::Element::TypeExpression:
    return this->computeIndentationTypeExpression();
  case CodeFormatter::Element::Expression:
    return this->computeIndentationExpression();
  case CodeFormatter::Element::Operator:
    return this->computeIndentationOperator();
  case CodeFormatter::Element::InAngleBrackets:
    return this->computeIndentationAngleBrackets();
  case CodeFormatter::Element::InParentheses:
    return this->computeIndentationInParentheses();
  case CodeFormatter::Element::InBrackets:
    return this->computeIndentationInBrackets();
  case CodeFormatter::Element::IfClause:
    return this->computeIndentationIfClause();
  case CodeFormatter::Element::IfWantsCodeBlock:
    return this->computeIndentationIfWantsCodeBlock();
  case CodeFormatter::Element::Quote:
    return this->computeIndentationQuote();
  case CodeFormatter::Element::EnumDefinition:
    return this->computeIndentationEnumDefinition();
  case CodeFormatter::Element::EnumDeclaration:
    return this->computeIndentationEnumDeclaration();
  case CodeFormatter::Element::CommaList:
    return this->computeIndentationCommaList();
  case CodeFormatter::Element::FunctionWithArgumentsAndInitializerList:
    return this->computeIndentationInitializerList();
  case CodeFormatter::Element::FunctionWithArguments:
    return this->computeIndentationFunctionWithArguments();
  case CodeFormatter::Element::Command:
    return this->computeIndentationCommand();
  case CodeFormatter::Element::CommandList:
    return this->computeIndentationCommandList();
  case CodeFormatter::Element::CommentCollection:
    return this->computeIndentationCommentCollection();
  case CodeFormatter::Element::Comment:
    return this->computeIndentationComment();
  case CodeFormatter::Element::FunctionDeclaration:
    return this->computeIndentationFunctionDeclaration();
  case CodeFormatter::Element::FunctionDefinition:
    return this->computeIndentationFunctionDefinition();
  case CodeFormatter::Element::GreaterThan:
    return this->computeIndentationGreaterThan();
  case CodeFormatter::Element::LessThan:
    return this->computeIndentationLessThan();
  case CodeFormatter::Element::TemplateClause:
    return this->computeIndentationTemplateClause();
  case CodeFormatter::Element::MacroLine:
    return this->computeIndentationMacroline();
  default:
    break;
  }
  if (this->content != "") {
    this->computeIndentationAtomic();
    return true;
  }
  this->computeIndentationBasic(0);
  return true;
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

bool CodeFormatter::Element::computeIndentationFunctionDeclaration() {
  for (int i = 1; i < this->children.size; i ++) {
    CodeFormatter::Element& previous = this->children[i - 1];
    if (previous.content == "," || previous.content == ":") {
      this->children[i].leftMostAtomUnderMe()->whiteSpaceBefore = 1;
    }
  }
  this->computeIndentationBasic(0);
  if (!this->containsNewLineAfterExcludingComments()) {
    return true;
  }
  this->resetWhitespaceRecursively();
  for (int i = 1; i < this->children.size; i ++) {
    CodeFormatter::Element& previous = this->children[i - 1];
    if (previous.content == "," || previous.content == ":") {
      previous.newLinesAfter = 1;
    }
  }
  this->computeIndentationBasic(0);
  return true;
}

bool CodeFormatter::Element::computeIndentationFunctionDefinition() {
  this->computeIndentationBasic(0);
  if (this->children.size != 2) {
    return true;
  }
  CodeFormatter::Element& functionDeclaration = this->children[0];
  CodeFormatter::Element& lastElementFunctionDeclaration =
  *functionDeclaration.rightMostAtomUnderMe();
  if (
    lastElementFunctionDeclaration.type == CodeFormatter::Element::ConstKeyWord
  ) {
    lastElementFunctionDeclaration.newLinesAfter = 0;
    this->children[1].leftMostAtomUnderMe()->whiteSpaceBefore = 1;
  } else if (
    lastElementFunctionDeclaration.columnFinal ==
    this->owner->maximumDesiredLineLength - 2 &&
    functionDeclaration.maximumDesiredLineLengthOverride <= 0
  ) {
    functionDeclaration.maximumDesiredLineLengthOverride =
    this->owner->maximumDesiredLineLength - 2;
    this->computeIndentationFunctionDefinition();
  }
  return true;
}

bool CodeFormatter::Element::computeIndentationCommentCollection() {
  for (int i = 0; i < this->children.size; i ++) {
    CodeFormatter::Element& current = this->children[i];
    current.indentationLevel = this->indentationLevel;
    current.computeIndentation();
    current.rightMostAtomUnderMe()->newLinesAfter = 1;
    current.leftMostAtomUnderMe()->whiteSpaceBefore = current.indentationLevel;
  }
  this->whiteSpaceBefore = 0;
  return true;
}

bool CodeFormatter::Element::computeIndentationComment() {
  if (this->children.size > 0) {
    return true;
  }
  if (this->previousAtom()->newLinesAfter > 0) {
    this->whiteSpaceBefore = this->indentationLevel;
  } else if (
    this->parent != nullptr &&
    this->parent->type == CodeFormatter::Element::TopLevel
  ) {
    this->leftMostAtomUnderMe()->whiteSpaceBefore = 0;
  } else {
    this->leftMostAtomUnderMe()->whiteSpaceBefore = 1;
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
  if (this->content == "// ") {
    this->content = "//";
  }
  StringRoutines::splitExcludeDelimiter(this->content, ' ', words);
  if (words.size == 0) {
    global << Logger::red << "Unexpected empty comment. " << Logger::endL;
    global.comments << "Unexpected empty comment. ";
    return true;
  }
  int currentOffset = this->offsetFromPrevious();
  List<std::string> lines;
  std::string currentWord;
  int maximumWidth = this->maximumLineLength();
  currentWord = words[0];
  for (int i = 1; i < words.size; i ++) {
    int incomingLength =
    currentOffset + currentWord.size() + 1 + words[i].size();
    if (incomingLength >= maximumWidth) {
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
    return true;
  }
  this->resetWhitespaceRecursively();
  this->content = "";
  this->children.clear();
  this->type = CodeFormatter::Element::CommentCollection;
  this->newLinesAfter = 0;
  for (int i = 0; i < lines.size; i ++) {
    CodeFormatter::Element next;
    next.initializePointers(this->owner, this, i);
    next.indentationLevel = this->indentationLevel;
    next.whiteSpaceBefore = this->indentationLevel;
    next.newLinesAfter = 1;
    next.type = CodeFormatter::Element::Comment;
    next.content = lines[i];
    this->addChild(next);
  }
  return true;
}

bool CodeFormatter::Element::computeIndentationCurlyBraceCommaDelimitedList() {
  this->computeIndentationBasic(0);
  if (this->children.size < 3) {
    return true;
  }
  if (!this->containsNewLineAfterExcludingComments()) {
    return true;
  }
  this->resetWhitespaceRecursively();
  this->children[0].indentationLevel = this->indentationLevel;
  this->children[0].newLinesAfter = 1;
  this->children[0].computeIndentation();
  int indentationChildren = this->indentationLevel + this->owner->tabLength;
  CodeFormatter::Element& list = this->children[1];
  list.indentationLevel = indentationChildren;
  list.computeIndentation();
  for (int i = 0; i < list.children.size; i ++) {
    CodeFormatter::Element& child = list.children[i];
    if (child.content == ",") {
      child.rightMostAtomUnderMe()->newLinesAfter = 1;
    }
  }
  list.indentationLevel = indentationChildren;
  list.computeIndentation();
  if (list.children.size > 0) {
    list.children.lastObject()->rightMostAtomUnderMe()->newLinesAfter = 1;
  }
  this->children.lastObject()->indentationLevel = this->indentationLevel;
  this->children.lastObject()->computeIndentation();
  this->children.lastObject()->whiteSpaceBefore = this->indentationLevel;
  return true;
}

bool CodeFormatter::Element::computeIndentationCodeBlock() {
  if (this->children.size < 2) {
    this->computeIndentationBasic(0);
    return true;
  }
  this->children[0].whiteSpaceBefore = 1;
  this->children[0].indentationLevel = this->indentationLevel;
  this->children[0].computeIndentation();
  if (this->children.size > 2) {
    this->children[0].newLinesAfter = 1;
  }
  if (this->children.size == 3) {
    if (
      this->children[1].type == CodeFormatter::Element::CommandList ||
      this->children[1].type == CodeFormatter::Element::CaseClauseList
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
  return true;
}

bool CodeFormatter::Element::computeIndentationEnumDeclaration() {
  this->computeIndentationBasic(0);
  return true;
}

bool CodeFormatter::Element::computeIndentationEnumDefinition() {
  if (this->children.size != 5) {
    this->computeIndentationBasic(0);
    return true;
  }
  this->children[0].indentationLevel = this->indentationLevel;
  this->children[0].computeIndentation();
  this->children[1].indentationLevel = this->indentationLevel;
  this->children[1].whiteSpaceBefore = 1;
  this->children[1].newLinesAfter = 1;
  CodeFormatter::Element& content = this->children[2];
  content.indentationLevel = this->indentationLevel + this->owner->tabLength;
  content.computeIndentation();
  if (content.type == CodeFormatter::Element::Type::CommaList) {
    if (content.children.size > 0) {
      content.rightMostAtomUnderMe()->newLinesAfter = 1;
    }
  }
  this->children[3].indentationLevel = this->indentationLevel;
  this->children[3].whiteSpaceBefore = this->indentationLevel;
  this->children[4].indentationLevel = this->indentationLevel;
  return true;
}

bool CodeFormatter::Element::computeIndentationControlWantsCodeBlock() {
  if (this->children.size != 2) {
    this->computeIndentationBasic(0);
    return true;
  }
  this->maximumDesiredLineLengthOverride =
  this->owner->maximumDesiredLineLength - 2;
  CodeFormatter::Element& keyword = this->children[0];
  CodeFormatter::Element& condition = this->children[1];
  keyword.maximumDesiredLineLengthOverride =
  this->maximumDesiredLineLengthOverride;
  condition.maximumDesiredLineLengthOverride =
  this->maximumDesiredLineLengthOverride;
  keyword.indentationLevel = this->indentationLevel;
  keyword.computeIndentation();
  condition.indentationLevel = this->indentationLevel;
  condition.leftMostAtomUnderMe()->whiteSpaceBefore = 1;
  condition.computeIndentation();
  return true;
}

bool CodeFormatter::Element::computeIndentationCaseClause() {
  for (int i = 0; i < this->children.size; i ++) {
    this->children[i].rightMostAtomUnderMe()->newLinesAfter = 1;
    this->children[i].indentationLevel = this->indentationLevel;
    this->children[i].computeIndentation();
  }
  return true;
}

bool CodeFormatter::Element::computeIndentationCaseClauseList() {
  for (CodeFormatter::Element& child : this->children) {
    child.rightMostAtomUnderMe()->newLinesAfter = 1;
    child.indentationLevel = this->indentationLevel;
    if (child.isComment()) {
      child.indentationLevel -= this->owner->tabLength;
      if (child.indentationLevel < 0) {
        child.indentationLevel = 0;
      }
    }
    child.computeIndentation();
  }
  return true;
}

bool CodeFormatter::Element::computeIndentationCaseClauseMultipleStart() {
  for (CodeFormatter::Element& child : this->children) {
    child.indentationLevel = this->indentationLevel;
    child.rightMostAtomUnderMe()->newLinesAfter = 1;
    child.computeIndentation();
  }
  return true;
}

bool CodeFormatter::Element::computeIndentationCaseClauseStart() {
  this->indentationLevel -= this->owner->tabLength;
  if (this->indentationLevel < 0) {
    this->indentationLevel = 0;
  }
  this->computeIndentationBasic(0);
  return true;
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
  for (int i = 0; i < this->newLinesAfter; i ++) {
    out << "&darr;";
  }
  for (int i = 0; i < this->children.size; i ++) {
    out << this->children[i].toStringHTMLTree();
  }
  out << "</div>";
  return out.str();
}

bool CodeFormatter::Element::computeIndentationCommandList() {
  if (
    this->parent->type != CodeFormatter::Element::CodeBlock &&
    this->parent->type != CodeFormatter::Element::CaseClause
  ) {
    for (int i = 0; i < this->children.size; i ++) {
      CodeFormatter::Element& current = this->children[i];
      current.indentationLevel = this->indentationLevel;
      current.computeIndentation();
    }
    return true;
  }
  for (int i = 0; i < this->children.size; i ++) {
    CodeFormatter::Element& current = this->children[i];
    current.rightMostAtomUnderMe()->newLinesAfter = 1;
    bool shouldIndentNegatively = false;
    if (current.type == CodeFormatter::Element::VisibilityClause) {
      shouldIndentNegatively = true;
    }
    current.indentationLevel = this->indentationLevel;
    if (shouldIndentNegatively) {
      current.indentationLevel -= this->owner->tabLength;
      if (current.indentationLevel < 0) {
        current.indentationLevel = 0;
      }
    }
    if (current.type == CodeFormatter::Element::Comment) {
      current.whiteSpaceBefore = this->indentationLevel;
    }
    if (
      i + 1 < this->children.size &&
      current.type == CodeFormatter::Element::ClassDefinition
    ) {
      current.rightMostAtomUnderMe()->newLinesAfter = 2;
    }
    current.computeIndentation();
  }
  return true;
}

void CodeFormatter::Element::resetWhitespaceRecursively() {
  this->newLinesAfter = 0;
  this->whiteSpaceBefore = 0;
  this->columnFinal = 0;
  for (int i = 0; i < this->children.size; i ++) {
    this->children[i].resetWhitespaceRecursively();
  }
}

bool CodeFormatter::Element::containsNewLineAfterExcludingComments() {
  for (int i = 0; i < this->children.size; i ++) {
    if (this->children[i].containsNewLineAfterExcludingComments()) {
      return true;
    }
  }
  if (
    this->type == CodeFormatter::Element::Type::Comment ||
    this->type == CodeFormatter::Element::Type::CommentMultiline
  ) {
    return false;
  }
  return this->newLinesAfter > 0;
}

bool CodeFormatter::Element::computeIndentationQuote() {
  for (int i = 0; i < this->children.size; i ++) {
    CodeFormatter::Element& child = this->children[i];
    if (i != this->children.size - 1) {
      child.newLinesAfter = 1;
    }
    if (i > 0) {
      CodeFormatter::Element* next = this->nextAtom();
      if (
        next != nullptr && (
          next->type == CodeFormatter::Element::Type::RightParenthesis ||
          next->type == CodeFormatter::Element::Type::Quote
        )
      ) {
        child.newLinesAfter = 1;
      }
    }
    child.indentationLevel = this->indentationLevel;
    child.computeIndentation();
  }
  if (this->content != "") {
    this->computeIndentationAtomic();
  }
  return true;
}

bool CodeFormatter::Element::operator>(const CodeFormatter::Element& other)
const {
  if (this->type > other.type) {
    return true;
  }
  if (this->type < other.type) {
    return false;
  }
  if (this->content > other.content) {
    return true;
  }
  if (this->content < other.content) {
    return false;
  }
  for (int i = 0; i < this->children.size; i ++) {
    if (i >= other.children.size) {
      return true;
    }
    CodeFormatter::Element & child = this->children[i];
    CodeFormatter::Element& otherChild = other.children[i];
    if (child > otherChild) {
      return true;
    }
    if (otherChild > child) {
      return false;
    }
  }
  return false;
}

bool CodeFormatter::Element::computeIndentationIfWantsCodeBlock() {
  if (this->children.size >= 2) {
    this->children[1].whiteSpaceBefore = 1;
  }
  this->computeIndentationBasic(0);
  CodeFormatter::Element* rightMostChild = this->rightMostAtomUnderMe();
  if (
    rightMostChild == nullptr ||
    rightMostChild->columnFinal < this->owner->maximumDesiredLineLength - 2
  ) {
    return true;
  }
  // We are exactly 2 characters from the end line, so we risk having code
  // formatting such as
  // if (something)
  // {
  // }
  // which doesn't look consistent.
  // Instead, we want:
  // if (
  //   something
  // ) {
  // }
  // for consistency.
  // Restrict line length and recompute.
  this->maximumDesiredLineLengthOverride =
  this->owner->maximumDesiredLineLength - 1;
  return this->computeIndentationBasic(0);
}

bool CodeFormatter::Element::computeIndentationIfClause() {
  this->computeIndentationBasic(0);
  for (int i = 0; i < this->children.size; i ++) {
    if (this->children[i].type == CodeFormatter::Element::Else) {
      this->children[i].whiteSpaceBefore = 1;
    }
  }
  return true;
}

bool CodeFormatter::Element::computeIndentationCommand() {
  STACK_TRACE("CodeFormatter::Element::computeIndentationCommand");
  for (int i = 0; i < this->children.size; i ++) {
    this->children[i].indentationLevel = this->indentationLevel;
    this->children[i].computeIndentation();
  }
  if (this->children.size == 0) {
    return true;
  }
  CodeFormatter::Element& last = *this->children.lastObject();
  if (last.content != ";") {
    return true;
  }
  if (last.whiteSpaceBefore != this->indentationLevel) {
    return true;
  }
  // We have a semicolon that is standing on a new line alone.
  int updatedMaximumLength = this->maximumLineLength() - 1;
  for (int i = 0; i < this->children.size - 1; i ++) {
    this->children[i].indentationLevel = this->indentationLevel;
    this->children[i].maximumDesiredLineLengthOverride = updatedMaximumLength;
    this->children[i].computeIndentation();
  }
  if (this->children.size > 1) {
    CodeFormatter::Element& secondToLast = *last.previousAtom();
    if (secondToLast.columnFinal < this->maximumLineLength()) {
      if (
        secondToLast.type != CodeFormatter::Element::CommentCollection &&
        secondToLast.type != CodeFormatter::Element::Comment &&
        secondToLast.type != CodeFormatter::Element::CommentMultiline
      ) {
        secondToLast.newLinesAfter = 0;
      }
    }
  }
  last.whiteSpaceBefore = 0;
  last.computeIndentation();
  return true;
}

bool CodeFormatter::Element::computeIndentationFunctionWithArguments() {
  bool result = this->computeIndentationBasic(0);
  return result;
}

bool CodeFormatter::Element::computeIndentationInitializerList() {
  return this->computeIndentationBasic(0);
}

bool CodeFormatter::Element::computeIndentationCommaList(bool forceNewLines) {
  bool mustSplitLines = forceNewLines;
  if (!mustSplitLines) {
    if (
      this->minimalSizeWithSpacebars() + this->offsetFromPrevious() >
      this->maximumLineLength()
    ) {
      mustSplitLines = true;
    }
    if (this->containsNewLineAfterExcludingComments()) {
      mustSplitLines = true;
    }
  }
  if (mustSplitLines) {
    CodeFormatter::Element* previous = this->previousAtom();
    if (previous != nullptr) {
      previous->newLinesAfter = 1;
    }
  }
  for (int i = 0; i < this->children.size; i ++) {
    CodeFormatter::Element& current = this->children[i];
    bool isCommaOrComment = current.content == "," || current.isComment();
    if (mustSplitLines) {
      current.resetWhitespaceRecursively();
    }
    current.indentationLevel = this->indentationLevel;
    if (current.isComment()) {
      current.whiteSpaceBefore = this->indentationLevel;
    }
    current.computeIndentation();
    if (mustSplitLines && (isCommaOrComment || i == this->children.size - 1)) {
      current.rightMostAtomUnderMe()->newLinesAfter = 1;
    }
  }
  if (
    !mustSplitLines &&
    !forceNewLines &&
    this->containsNewLineAfterExcludingComments()
  ) {
    return this->computeIndentationCommaList(true);
  }
  return true;
}

bool CodeFormatter::Element::computeIndentationExpression() {
  this->computeIndentationBasic(0);
  if (!this->containsNewLineAfterExcludingComments()) {
    return true;
  }
  this->breakExpression();
  return true;
}

bool CodeFormatter::Element::computeIndentationBasic(int startingIndex) {
  for (int i = startingIndex; i < this->children.size; i ++) {
    this->children[i].indentationLevel = this->indentationLevel;
    this->children[i].computeIndentation();
  }
  return true;
}

bool CodeFormatter::Element::isOfTypeOrCommandOfType(
  CodeFormatter::Element::Type input
) {
  if (this->type == input) {
    return true;
  }
  if (this->type != CodeFormatter::Element::Command) {
    return false;
  }
  if (this->children.size <= 0) {
    return false;
  }
  return this->children[0].type == input;
}

bool CodeFormatter::Element::shouldAddExtraLineInTopLevel(
  CodeFormatter::Element& next
) {
  if (
    this->type == CodeFormatter::Element::IncludeLine &&
    next.type != CodeFormatter::Element::IncludeLine
  ) {
    return true;
  }
  if (
    this->type == CodeFormatter::Element::MacroLine &&
    next.type != CodeFormatter::Element::MacroLine
  ) {
    return true;
  }
  if (
    this->isOfTypeOrCommandOfType(CodeFormatter::Element::ClassDeclaration) &&
    !next.isOfTypeOrCommandOfType(CodeFormatter::Element::ClassDeclaration)
  ) {
    return true;
  }
  if (this->isOfTypeOrCommandOfType(CodeFormatter::Element::ClassDefinition)) {
    return true;
  }
  if (
    this->isOfTypeOrCommandOfType(CodeFormatter::Element::FunctionDefinition)
  ) {
    return true;
  }
  if (
    next.isOfTypeOrCommandOfType(CodeFormatter::Element::FunctionDefinition) &&
    !this->isComment()
  ) {
    return true;
  }
  if (this->type == CodeFormatter::Element::EnumDefinition) {
    return true;
  }
  return false;
}

bool CodeFormatter::Element::computeIndentationTopLevel() {
  STACK_TRACE("CodeFormatter::Element::computeIndentationTopLevel");
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
    if (i + 1 < this->children.size) {
      addExtraLine =
      current.shouldAddExtraLineInTopLevel(this->children[i + 1]);
    }
    if (addExtraLine) {
      rightMostAtom->newLinesAfter = 2;
    }
  }
  return true;
}

CodeFormatter::Element* CodeFormatter::Element::previousSibling() {
  if (this->parent == nullptr || this->indexInParent <= 0) {
    return nullptr;
  }
  return &this->parent->children[this->indexInParent - 1];
}

CodeFormatter::Element* CodeFormatter::Element::previousAtom() const {
  if (this->parent == nullptr) {
    return nullptr;
  }
  if (this->indexInParent == 0) {
    return this->parent->previousAtom();
  }
  return
  this->parent->children[this->indexInParent - 1].rightMostAtomUnderMe();
}

CodeFormatter::Element* CodeFormatter::Element::nextAtom() const {
  if (this->parent == nullptr) {
    return nullptr;
  }
  if (this->indexInParent + 1 >= this->parent->children.size) {
    return this->parent->nextAtom();
  }
  return this->parent->children[this->indexInParent + 1].leftMostAtomUnderMe();
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
  if (this->currentChar == '"' && !this->flagPreviousIsStandaloneBackSlash) {
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
  if (this->currentChar == '\'' && !this->flagPreviousIsStandaloneBackSlash) {
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
  if (this->currentWord.type != CodeFormatter::Element::CommentMultiline) {
    return false;
  }
  this->currentWord.content += this->currentChar;
  if (
    this->currentChar == '/' &&
    this->currentWord.content[this->currentWord.content.size() - 2] == '*'
  ) {
    this->addCurrentWord();
  }
  return true;
}

bool CodeFormatter::Words::extractCodeElements(std::stringstream* comments) {
  STACK_TRACE("CodeFormatter::Words::extractCodeElements");
  (void) comments;
  std::string& inputCode = this->owner->inputCode;
  this->elements.setExpectedSize(static_cast<signed>(inputCode.size()));
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
  CodeFormatter::Element fileEnd;
  fileEnd.type = CodeFormatter::Element::EndFile;
  this->elements.addOnTop(fileEnd);
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
  const std::string& output,
  const std::string& outputOnFail,
  std::stringstream* comments
) {
  STACK_TRACE("CodeFormatter::initializeFileNames");
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
  if (output != "") {
    this->outputFileName = output;
  } else {
    this->outputFileName = this->inputFileName + ".new";
  }
  this->outputOnFailFileName = outputOnFail;
  if (this->outputOnFailFileName == "") {
    this->outputOnFailFileName = this->inputFileName + ".fail";
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
  STACK_TRACE("CodeFormatter::toStringLinks");
  std::stringstream out;
  bool parsingFailed = !this->parsingSucceeded();
  if (parsingFailed) {
    out << "<b style='color:red'>";
  }
  out << "<a href = '" << this->inputFileName << "'";
  if (parsingFailed) {
    out << " style='color:red'";
  }
  out << ">" << this->inputFileName << "</a>&rarr; ";
  out << "<a href = '" << this->outputFileName << "'";
  if (parsingFailed) {
    out << " style='color:red'";
  }
  out << ">" << this->outputFileName << "</a>. ";
  if (!this->parsingSucceeded()) {
    out << "</b>";
  }
  return out.str();
}

bool CodeFormatter::formatCalculatorInPlace() {
  std::stringstream comments;
  CodeFormatter::formatCPPDirectory("src/", true, &comments);
  global << comments.str() << Logger::endL;
  return true;
}

bool CodeFormatter::formatCPPDirectory(
  const std::string& inputDirectory, bool inPlace, std::stringstream* comments
) {
  STACK_TRACE("CodeFormatter::formatCPPDirectory");
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
  List<std::string> allFiles;
  List<std::string> newFileNames;
  List<std::string> oldFileNames;
  List<std::string> allFileExtensions;
  if (
    !FileOperations::getFolderFileNamesVirtual(
      directory, allFiles, &allFileExtensions, false, false, comments
    )
  ) {
    return false;
  }
  std::string newDirectory;
  if (inPlace) {
    newDirectory = directory;
  } else {
    newDirectory = "output/" + directory;
  }
  newFileNames.setExpectedSize(allFiles.size);
  oldFileNames.setExpectedSize(allFiles.size);
  for (int i = 0; i < allFiles.size; i ++) {
    if (allFiles[i] == "temp.cpp") {
      // This is a working file.
      continue;
    }
    if (allFileExtensions[i] == ".cpp" || allFileExtensions[i] == ".h") {
      oldFileNames.addOnTop(directory + allFiles[i]);
      newFileNames.addOnTop(newDirectory + allFiles[i]);
    }
  }
  oldFileNames.quickSortAscending(nullptr, &newFileNames);
  for (int i = 0; i < newFileNames.size; i ++) {
    std::string fileNameOnFail = newFileNames[i];
    if (inPlace) {
      fileNameOnFail += ".fail";
    }
    CodeFormatter formatter;
    if (
      !formatter.formatCPPSourceCode(
        oldFileNames[i], newFileNames[i], fileNameOnFail, comments, false
      )
    ) {
      return false;
    }
    if (comments != nullptr) {
      *comments << "<br>";
    }
    if (inPlace) {
      global << "Formatted: " << oldFileNames[i] << " ... ";
      if (formatter.parsingSucceeded()) {
        global << Logger::green << "OK.";
      } else {
        global << Logger::red << "FAILED";
      }
      global << Logger::endL;
    }
  }
  return true;
}

bool CodeFormatter::parsingSucceeded() const {
  return this->processor.code.children.size <= this->dummyElements + 1;
}

bool CodeFormatter::formatCPPSourceCode(
  const std::string& inputFileName,
  const std::string& output,
  const std::string& outputOnFail,
  std::stringstream* comments,
  bool logDebugInfo
) {
  STACK_TRACE("CodeFormatter::formatCPPSourceCode");
  if (
    !this->initializeFileNames(inputFileName, output, outputOnFail, comments)
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
  this->sortHeaders(this->processor.code);
  this->wirePointersRecursively(this->processor.code, nullptr, - 1);
  if (logDebugInfo && comments != nullptr) {
    *comments << "<b>Parsing log follows.</b><br>" << this->processor.debugLog;
  }
  if (this->parsingSucceeded()) {
    out << this->processor.code.format();
  } else {
    out << "FAILED_TO_PARSE;\n";
    for (
      int i = this->dummyElements; i < this->processor.code.children.size; i
      ++
    ) {
      this->processor.code.children[i].toStringContentOnly(out);
      out << "\n";
    }
  }
  if (logDebugInfo && comments != nullptr) {
    *comments
    << "<hr>Code element tree follows<br>"
    << this->processor.code.toStringHTMLTree();
  }
  this->transformedContent = out.str();
  if (!this->writeFormattedCode(comments)) {
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
  const std::string& overridingOperator, const List<std::string>& overridden
) {
  HashedList<std::string, HashFunctions::hashFunction>& overridenHashed =
  this->operatorOverrides.getValueCreateEmpty(overridingOperator);
  overridenHashed.addListOnTop(overridden);
}

CodeFormatter::CodeFormatter() {
  this->separatorCharacters = "?^~(){} \t\n\r,:;.*&+-/%[]#\"'=><!|";
  this->doesntNeedSpaceToTheLeft = "&,:.()";
  this->doesntNeedSpaceToTheRight = "():.";
  this->pairsNotSeparatedBySpace.addOnTop(List<std::string>({"&", "&"}));
  this->pairsNotSeparatedBySpace.addOnTop(List<std::string>({"&", "*"}));
  this->pairsNotSeparatedBySpace.addOnTop(List<std::string>({"*", "&"}));
  List<std::string> relations = List<std::string>(
    {"=", "==", ">=", ">", "<=", "!=", "*=", "+=", "-=", "/=", "^="}
  );
  List<std::string> andAndOr = List<std::string>({"&&", "||"});
  List<std::string> arithmeticOperations = List<std::string>(
    {"+", "-", "/", "*"}
  );
  List<std::string> bitShift = List<std::string>({"<<", ">>"});
  List<std::string> andOrRelations = relations;
  andOrRelations.addListOnTop(andAndOr);
  this->addOperatorOverride("+", andOrRelations);
  this->addOperatorOverride("+", List<std::string>({"<<"}));
  this->addOperatorOverride("/", List<std::string>({"<<"}));
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
  this->addOperatorOverride("*", andOrRelations);
  this->addOperatorOverride("*", List<std::string>({"+", "-", "<<"}));
  // Example: List<A * may extend to List < A*B or to List<A*>.
  this->addOperatorOverride("*", List<std::string>({"<"}));
  this->addOperatorOverride(">=", andAndOr);
  this->addOperatorOverride("<=", andAndOr);
  this->addOperatorOverride(">", andAndOr);
  // Less than overrides just about anything as it can
  // be extended to template argument in most cases.
  this->addOperatorOverride("<", List<std::string>({"<"}));
  this->addOperatorOverride("<", List<std::string>({"&&", "||", "="}));
  this->addOperatorOverride("?", relations);
  this->addOperatorOverride("<", arithmeticOperations);
  this->addOperatorOverride("<", andOrRelations);
  this->addOperatorOverride("<", List<std::string>({".", "->"}));
  this->addOperatorOverride("::", List<std::string>({".", "->"}));
  this->addOperatorOverride("!=", andAndOr);
  this->addOperatorOverride("!", andAndOr);
  this->addOperatorOverride("||", List<std::string>({"->", "."}));
  this->addOperatorOverride("&&", List<std::string>({"->", "."}));
  this->elementTypes.setKeyValue(":", CodeFormatter::Element::Colon);
  this->elementTypes.setKeyValue(";", CodeFormatter::Element::SemiColon);
  this->elementTypes.setKeyValue("(", CodeFormatter::Element::LeftParenthesis);
  this->elementTypes.setKeyValue(
    ")", CodeFormatter::Element::RightParenthesis
  );
  this->elementTypes.setKeyValue("{", CodeFormatter::Element::LeftCurlyBrace);
  this->elementTypes.setKeyValue("}", CodeFormatter::Element::RightCurlyBrace);
  this->elementTypes.setKeyValue("[", CodeFormatter::Element::LeftBracket);
  this->elementTypes.setKeyValue("]", CodeFormatter::Element::RightBracket);
  this->elementTypes.setKeyValue("&", CodeFormatter::Element::Ampersand);
  this->elementTypes.setKeyValue("*", CodeFormatter::Element::Star);
  this->elementTypes.setKeyValue("!", CodeFormatter::Element::Exclamation);
  this->elementTypes.setKeyValue(",", CodeFormatter::Element::Comma);
  this->elementTypes.setKeyValue(">", CodeFormatter::Element::GreaterThan);
  this->elementTypes.setKeyValue("<", CodeFormatter::Element::LessThan);
  this->elementTypes.setKeyValue("~", CodeFormatter::Element::Tilde);
  this->elementTypes.setKeyValue("?", CodeFormatter::Element::QuestionMark);
  this->elementTypes.setKeyValue("bitand", CodeFormatter::Element::Operator);
  this->elementTypes.setKeyValue("bitor", CodeFormatter::Element::Operator);
  this->elementTypes.setKeyValue("and", CodeFormatter::Element::Operator);
  this->elementTypes.setKeyValue("or", CodeFormatter::Element::Operator);
  this->elementTypes.setKeyValue("xor", CodeFormatter::Element::Operator);
  this->elementTypes.setKeyValue("if", CodeFormatter::Element::If);
  this->elementTypes.setKeyValue("else", CodeFormatter::Element::Else);
  this->elementTypes.setKeyValue("return", CodeFormatter::Element::Return);
  this->elementTypes.setKeyValue("new", CodeFormatter::Element::NewKeyWord);
  this->elementTypes.setKeyValue(
    "delete", CodeFormatter::Element::DeleteKeyWord
  );
  this->elementTypes.setKeyValue(
    "template", CodeFormatter::Element::TemplateKeyWord
  );
  this->elementTypes.setKeyValue(
    "const", CodeFormatter::Element::ConstKeyWord
  );
  this->elementTypes.setKeyValue(
    "operator", CodeFormatter::Element::OperatorKeyWord
  );
  this->elementTypes.setKeyValue(
    "static", CodeFormatter::Element::TypeAdjectiveKeyWord
  );
  this->elementTypes.setKeyValue(
    "virtual", CodeFormatter::Element::TypeAdjectiveKeyWord
  );
  this->elementTypes.setKeyValue(
    "extern", CodeFormatter::Element::TypeAdjectiveKeyWord
  );
  this->elementTypes.setKeyValue("do", CodeFormatter::Element::DoKeyWord);
  this->elementTypes.setKeyValue(
    "inline", CodeFormatter::Element::TypeAdjectiveKeyWord
  );
  this->elementTypes.setKeyValue(
    "typedef", CodeFormatter::Element::TypeAdjectiveKeyWord
  );
  this->elementTypes.setKeyValue(
    "friend", CodeFormatter::Element::FriendKeyWord
  );
  this->elementTypes.setKeyValue(
    "mutable", CodeFormatter::Element::TypeAdjectiveKeyWord
  );
  this->elementTypes.setKeyValue("case", CodeFormatter::Element::CaseKeyWord);
  this->elementTypes.setKeyValue(
    "default", CodeFormatter::Element::DefaultKeyWord
  );
  this->elementTypes.setKeyValue(
    "public", CodeFormatter::Element::VisibilityKeyWord
  );
  this->elementTypes.setKeyValue(
    "private", CodeFormatter::Element::VisibilityKeyWord
  );
  this->elementTypes.setKeyValue(
    "protected", CodeFormatter::Element::VisibilityKeyWord
  );
  this->elementTypes.setKeyValue(
    "union", CodeFormatter::Element::UnionKeyWord
  );
  this->elementTypes.setKeyValue(
    "class", CodeFormatter::Element::ClassKeyWord
  );
  this->elementTypes.setKeyValue(
    "struct", CodeFormatter::Element::StructKeyWord
  );
  this->elementTypes.setKeyValue(
    "typename", CodeFormatter::Element::TypenameKeyWord
  );
  this->elementTypes.setKeyValue("enum", CodeFormatter::Element::EnumKeyWord);
  this->elementTypes.setKeyValue("try", CodeFormatter::Element::TryKeyWord);
  this->elementTypes.setKeyValue(
    "catch", CodeFormatter::Element::CatchKeyWord
  );
  this->typeKeyWords.addListOnTop(
    List<std::string>(
      {"bool", "void", "char", "signed", "unsigned", "int", "long"}
    )
  );
  this->controlKeyWords.addListOnTop(
    List<std::string>({"for", "while", "switch"})
  );
  this->needsSpaceToTheRightContainer.addListOnTop(
    List<std::string>({"for", "if", "while", "catch"})
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
        "^",
        "%",
        ".",
        "->",
        "==",
        ">=",
        "<=",
        "!=",
        "*=",
        "/=",
        "<<=",
        "%=",
        "+=",
        "^=",
        "-=",
        ">>",
        "<<",
        "&&",
        "||",
        "bitand",
        "bitor",
        "and",
        "or",
        "xor"
      })
  );
  this->maximumDesiredLineLength = 80;
  this->indexCurrentlyConsumed = 0;
  this->separatorCharactersMap.initializeFillInObject(256, false);
  for (unsigned i = 0; i < this->separatorCharacters.size(); i ++) {
    this->separatorCharactersMap[
      static_cast<signed>(this->separatorCharacters[i])
    ] =
    true;
  }
  for (unsigned i = 0; i < this->doesntNeedSpaceToTheLeft.size(); i ++) {
    std::string incoming = this->doesntNeedSpaceToTheLeft.substr(i, 1);
    this->doesntNeedSpaceToTheLeftContainer.addOnTop(incoming);
  }
  for (unsigned i = 0; i < this->doesntNeedSpaceToTheRight.size(); i ++) {
    std::string incoming = this->doesntNeedSpaceToTheRight.substr(i, 1);
    this->doesntNeedSpaceToTheRightContainer.addOnTop(incoming);
  }
}

CodeFormatter::Processor::Processor() {
  this->owner = nullptr;
  this->flagPrepareReport = false;
  this->flagExceededReportSize = false;
  this->dummy.type = CodeFormatter::Element::Dummy;
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
  STACK_TRACE("CodeFormatter::Processor::consumeOneElement");
  this->lastRuleName = "consume";
  this->stack.addOnTop(incoming);
  this->appendLog();
  int stackSizeToImprove = this->stack.size;
  int rulesSinceImprovement = 0;
  while (this->applyOneRule()) {
    this->appendLog();
    this->lastRuleName = "";
    rulesSinceImprovement ++;
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
  STACK_TRACE("CodeFormatter::Processor::applyOneRule");
  CodeFormatter::Element& last = this->stack[this->stack.size - 1];
  CodeFormatter::Element& secondToLast = this->stack[this->stack.size - 2];
  CodeFormatter::Element& thirdToLast = this->stack[this->stack.size - 3];
  CodeFormatter::Element& fourthToLast = this->stack[this->stack.size - 4];
  CodeFormatter::Element& fifthToLast = this->stack[this->stack.size - 5];
  CodeFormatter::Element& sixthToLast = this->stack[this->stack.size - 6];
  if (secondToLast.content == "#" && last.content == "include") {
    secondToLast.content += last.content;
    secondToLast.type = CodeFormatter::Element::IncludeLineStart;
    return this->removeLast();
  }
  if (
    secondToLast.content == "#" && (
      last.content == "define" ||
      last.content == "ifdef" ||
      last.content == "ifndef" ||
      last.content == "else" ||
      last.content == "endif" ||
      last.content == "if"
    )
  ) {
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
    if (
      secondToLast.children.size == 0 &&
      StringRoutines::stringEndsWith(secondToLast.content, "\\")
    ) {
      this->lastRuleName = "extend macro line";
      secondToLast.content += last.content;
      return this->removeLast();
    }
    this->lastRuleName = "end macro line";
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
    secondToLast.type == CodeFormatter::Element::Quote &&
    last.type == CodeFormatter::Element::Quote
  ) {
    this->lastRuleName = "quote quote";
    if (secondToLast.content == "") {
      secondToLast.addChild(last);
    } else {
      secondToLast.makeFrom2(
        CodeFormatter::Element::Quote, secondToLast, last
      );
    }
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
    fifthToLast.isIdentifierOrAtom() &&
    fourthToLast.type == CodeFormatter::Element::LessThan &&
    thirdToLast.isExpressionIdentifierOrAtom() &&
    secondToLast.isStarOrAmpersand() &&
    last.type == CodeFormatter::Element::GreaterThan
  ) {
    this->lastRuleName = "type less than type star greater than";
    fifthToLast.makeFrom5(
      CodeFormatter::Element::TypeExpression,
      fifthToLast,
      fourthToLast,
      thirdToLast,
      secondToLast,
      last
    );
    return this->removeLast(4);
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
  if (
    thirdToLast.isComment() &&
    secondToLast.type == CodeFormatter::Element::Expression
  ) {
    this->lastRuleName = "comment expression";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::Expression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::OperatorKeyWord && (
      secondToLast.type == CodeFormatter::Element::LeftBracket ||
      secondToLast.type == CodeFormatter::Element::LeftParenthesis
    ) && (
      last.type == CodeFormatter::Element::RightBracket ||
      last.type == CodeFormatter::Element::RightParenthesis
    )
  ) {
    this->lastRuleName = "operator brackets";
    thirdToLast.makeFrom3(
      CodeFormatter::Element::Identifier, thirdToLast, secondToLast, last
    );
    return this->removeLast(2);
  }
  if (
    !thirdToLast.isOperator() &&
    thirdToLast.type != CodeFormatter::Element::QuestionMark &&
    secondToLast.type == CodeFormatter::Element::FunctionWithArguments &&
    last.type == CodeFormatter::Element::Colon
  ) {
    this->lastRuleName = "functionWithArguments colon";
    secondToLast.makeFrom2(
      CodeFormatter::Element::FunctionWithArgumentsAndInitializerList,
      secondToLast,
      last
    );
    return this->removeLast();
  }
  if (
    thirdToLast.type ==
    CodeFormatter::Element::FunctionWithArgumentsAndInitializerList &&
    secondToLast.isExpressionIdentifierAtomOrFunctionWithArguments() &&
    last.type == CodeFormatter::Element::Comma
  ) {
    this->lastRuleName =
    "functionWithArgumentsAndInitializerList expression comma";
    thirdToLast.addChild(secondToLast);
    thirdToLast.addChild(last);
    return this->removeLast(2);
  }
  if (
    thirdToLast.type ==
    CodeFormatter::Element::FunctionWithArgumentsAndInitializerList &&
    secondToLast.isExpressionIdentifierAtomOrFunctionWithArguments() &&
    last.type == CodeFormatter::Element::LeftCurlyBrace
  ) {
    this->lastRuleName = "function colon expression leftCurlyBrace";
    thirdToLast.addChild(secondToLast);
    thirdToLast.type = CodeFormatter::Element::FunctionDeclaration;
    return this->removeBelowLast(1);
  }
  if ((
      thirdToLast.type == CodeFormatter::Element::CaseClauseStart ||
      thirdToLast.type == CodeFormatter::Element::CaseClauseMultipleStart
    ) && (secondToLast.isCommandListOrCommand()) && (
      last.type == CodeFormatter::Element::CaseKeyWord ||
      last.type == CodeFormatter::Element::CaseClauseStart ||
      last.type == CodeFormatter::Element::CaseClauseMultipleStart ||
      last.type == CodeFormatter::Element::RightCurlyBrace ||
      last.type == CodeFormatter::Element::DefaultKeyWord
    )
  ) {
    this->lastRuleName = "case clause start finalized";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::CaseClause, thirdToLast, secondToLast
    );
    thirdToLast.makeFrom1(CodeFormatter::Element::CaseClauseList, thirdToLast);
    return this->removeBelowLast(1);
  }
  if ((
      secondToLast.type == CodeFormatter::Element::CaseClauseStart && (
        last.type == CodeFormatter::Element::CaseClauseStart ||
        last.isComment()
      )
    )
  ) {
    this->lastRuleName = "two case clause starts";
    secondToLast.makeFrom2(
      CodeFormatter::Element::CaseClauseMultipleStart, secondToLast, last
    );
    return this->removeLast();
  }
  if ((
      thirdToLast.type == CodeFormatter::Element::CaseClauseStart ||
      thirdToLast.type == CodeFormatter::Element::CaseClauseMultipleStart
    ) && (
      secondToLast.type == CodeFormatter::Element::CaseClauseStart ||
      secondToLast.type == CodeFormatter::Element::CaseClauseMultipleStart ||
      secondToLast.isComment()
    )
  ) {
    this->lastRuleName = "two case clause starts X";
    thirdToLast.appendType(
      secondToLast, CodeFormatter::Element::CaseClauseMultipleStart
    );
    return this->removeBelowLast(1);
  }
  if ((
      thirdToLast.type == CodeFormatter::Element::CaseClauseMultipleStart && (
        secondToLast.type == CodeFormatter::Element::CaseClauseStart ||
        secondToLast.isComment()
      )
    )
  ) {
    this->lastRuleName = "case clause multiple case clause start";
    thirdToLast.addChild(secondToLast);
    return this->removeBelowLast(1);
  }
  if ((
      thirdToLast.type == CodeFormatter::Element::CaseClauseList ||
      thirdToLast.type == CodeFormatter::Element::CommandList
    ) &&
    secondToLast.type == CodeFormatter::Element::CaseClauseList
  ) {
    this->lastRuleName = "add case clause lists";
    thirdToLast.type = CodeFormatter::Element::CaseClauseList;
    thirdToLast.children.addListOnTop(secondToLast.children);
    return this->removeBelowLast(1);
  }
  if (secondToLast.type == CodeFormatter::Element::Quote) {
    this->lastRuleName = "quote to expression";
    secondToLast.makeFrom1(CodeFormatter::Element::Expression, secondToLast);
    return true;
  }
  if (last.type == CodeFormatter::Element::SingleQuote) {
    this->lastRuleName = "single quote to expression";
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
    secondToLast.appendType(last, CodeFormatter::Element::CommentCollection);
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
    secondToLast.makeFrom1(CodeFormatter::Element::Expression, secondToLast);
    return true;
  }
  if (
    fifthToLast.type == CodeFormatter::Element::TryKeyWord &&
    fourthToLast.type == CodeFormatter::Element::CodeBlock &&
    thirdToLast.type == CodeFormatter::Element::CatchKeyWord &&
    secondToLast.type == CodeFormatter::Element::InParentheses &&
    last.type == CodeFormatter::Element::CodeBlock
  ) {
    this->lastRuleName = "try codeBlock catch inParentheses codeBlock";
    fifthToLast.makeFrom5(
      CodeFormatter::Element::Command,
      fifthToLast,
      fourthToLast,
      thirdToLast,
      secondToLast,
      last
    );
    return this->removeLast(4);
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
    thirdToLast.type == CodeFormatter::Element::ControlWantsCodeBlock &&
    secondToLast.isCodeBlockOrCommand()
  ) {
    this->lastRuleName = "control wants code block command";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::Command, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
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
    this->lastRuleName = "ifClause to command";
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
    thirdToLast.type == CodeFormatter::Element::TemplateClause &&
    secondToLast.type == CodeFormatter::Element::TypeAndIdentifier
  ) {
    this->lastRuleName = "templateClause typeIndentifier";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeAndIdentifier, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TemplateClause &&
    secondToLast.type == CodeFormatter::Element::TypeExpression
  ) {
    this->lastRuleName = "templateClause typeExpression";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TemplateClause &&
    secondToLast.isIdentifierOrAtom() &&
    last.type == CodeFormatter::Element::LeftParenthesis
  ) {
    this->lastRuleName = "templateClause identifier leftParenthesis";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeAndIdentifier, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TemplateClause &&
    secondToLast.type == CodeFormatter::Element::FunctionDeclaration
  ) {
    this->lastRuleName = "templateClause functionDeclaration";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::FunctionDeclaration, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TemplateClause &&
    secondToLast.type == CodeFormatter::Element::ClassDeclaration
  ) {
    // Must come before object declaration.
    this->lastRuleName = "templateClause classDeclaration";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::ClassDeclaration, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::FriendKeyWord &&
    secondToLast.type == CodeFormatter::Element::ClassDeclaration
  ) {
    this->lastRuleName = "friend class declaration";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::ClassDeclaration, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TypeAdjectiveKeyWord &&
    secondToLast.type == CodeFormatter::Element::ClassDeclaration
  ) {
    this->lastRuleName = "typeAdjective classDeclaration";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::ClassDeclaration, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if ((
      secondToLast.type == CodeFormatter::Element::ClassDeclaration ||
      secondToLast.type == CodeFormatter::Element::FunctionDeclaration
    ) &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    // Must come after template clause+class declaration.
    this->lastRuleName = "object declaration ;";
    secondToLast.makeFrom2(
      CodeFormatter::Element::Command, secondToLast, last
    );
    return this->removeLast();
  }
  if (thirdToLast.type == CodeFormatter::Element::Tilde) {
    this->lastRuleName = "tilde to operator";
    thirdToLast.type = CodeFormatter::Element::Operator;
    return true;
  }
  if (
    secondToLast.type == CodeFormatter::Element::Tilde &&
    last.type == CodeFormatter::Element::Atom
  ) {
    this->lastRuleName = "tilde atom";
    secondToLast.makeFrom2(
      CodeFormatter::Element::Identifier, secondToLast, last
    );
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
    last.isIdentifierOrAtom()
  ) {
    this->lastRuleName = "identifier::atom";
    thirdToLast.appendType(secondToLast, CodeFormatter::Element::Identifier);
    thirdToLast.appendType(last, CodeFormatter::Element::Identifier);
    return this->removeLast(2);
  }
  if (
    fourthToLast.isOperator() &&
    thirdToLast.type == CodeFormatter::Element::DoubleColon &&
    secondToLast.type == CodeFormatter::Element::TypeExpression &&
    last.type == CodeFormatter::Element::LeftParenthesis
  ) {
    this->lastRuleName = "operator ::typeExpression";
    thirdToLast.appendType(secondToLast, CodeFormatter::Element::Identifier);
    return this->removeBelowLast(1);
  }
  if ((
      fourthToLast.isOperator() ||
      fourthToLast.type == CodeFormatter::Element::Return ||
      fourthToLast.type == CodeFormatter::Element::TypeKeyWord
    ) &&
    thirdToLast.type == CodeFormatter::Element::DoubleColon &&
    secondToLast.isIdentifierOrAtom() &&
    last.type == CodeFormatter::Element::LeftParenthesis
  ) {
    this->lastRuleName = "X ::identifier";
    thirdToLast.appendType(secondToLast, CodeFormatter::Element::Identifier);
    return this->removeBelowLast(1);
  }
  if ((thirdToLast.content == "." || thirdToLast.content == "->") &&
    secondToLast.type == CodeFormatter::Element::DoubleColon &&
    last.isIdentifierOrAtom()
  ) {
    this->lastRuleName = "dot double colon identifier";
    secondToLast.makeFrom2(
      CodeFormatter::Element::Identifier, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    fourthToLast.isIdentifierOrAtom() &&
    thirdToLast.type == CodeFormatter::Element::DoubleColon &&
    secondToLast.isIdentifierOrAtom()
  ) {
    this->lastRuleName = "identifier::atom X";
    fourthToLast.appendType(thirdToLast, CodeFormatter::Element::Identifier);
    fourthToLast.appendType(secondToLast, CodeFormatter::Element::Identifier);
    return this->removeBelowLast(2);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TypeExpression &&
    secondToLast.type == CodeFormatter::Element::DoubleColon &&
    last.isIdentifierOrAtom()
  ) {
    this->lastRuleName = "typeExpression::identifier";
    thirdToLast.addChild(secondToLast);
    thirdToLast.addChild(last);
    return this->removeLast(2);
  }
  if (
    fifthToLast.type != CodeFormatter::Element::Return && (
      fourthToLast.isTypeWordOrTypeExpression() ||
      fourthToLast.isIdentifierOrAtom()
    ) &&
    thirdToLast.isStarOrAmpersand() &&
    secondToLast.type == CodeFormatter::Element::TypeExpression &&
    last.type == CodeFormatter::Element::LeftParenthesis
  ) {
    this->lastRuleName =
    "typeExpression ampersand typeExpression leftParenthesis";
    fourthToLast.makeFrom3(
      CodeFormatter::Element::TypeAndIdentifier,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(2);
  }
  if (
    fourthToLast.type == CodeFormatter::Element::TypeExpression &&
    thirdToLast.type == CodeFormatter::Element::DoubleColon &&
    secondToLast.isIdentifierOrAtom()
  ) {
    this->lastRuleName = "typeExpression::atom X";
    fourthToLast.addChild(thirdToLast);
    fourthToLast.addChild(secondToLast);
    return this->removeBelowLast(2);
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
    secondToLast.type == CodeFormatter::Element::VisibilityKeyWord &&
    last.type == CodeFormatter::Element::Colon
  ) {
    this->lastRuleName = "visibility clause";
    secondToLast.makeFrom2(
      CodeFormatter::Element::VisibilityClause, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::VisibilityKeyWord &&
    last.isExpressionIdentifierOrAtom()
  ) {
    this->lastRuleName = "visibility of expression";
    secondToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    fourthToLast.type == CodeFormatter::Element::ClassDeclaration &&
    thirdToLast.type == CodeFormatter::Element::Colon &&
    secondToLast.isTypeWordOrTypeExpression() &&
    last.type == CodeFormatter::Element::LeftCurlyBrace
  ) {
    this->lastRuleName = "classDeclaration colon public clause";
    fourthToLast.makeFrom3(
      CodeFormatter::Element::ClassDeclaration,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(2);
  }
  if (
    fourthToLast.type != CodeFormatter::Element::Dummy &&
    thirdToLast.type == CodeFormatter::Element::LeftBracket &&
    this->isNoReturn(secondToLast) &&
    last.type == CodeFormatter::Element::RightBracket
  ) {
    this->lastRuleName = "no return identifier";
    if (fourthToLast.content != "") {
      if (fourthToLast.type == CodeFormatter::Element::Atom) {
        fourthToLast.makeFrom1(
          CodeFormatter::Element::Identifier, fourthToLast
        );
      } else {
        fourthToLast.makeFrom1(fourthToLast.type, fourthToLast);
      }
    }
    fourthToLast.addChild(thirdToLast);
    fourthToLast.addChild(secondToLast);
    fourthToLast.addChild(last);
    return this->removeLast(3);
  }
  if ((
      fourthToLast.type == CodeFormatter::Element::ClassDeclaration ||
      fourthToLast.isTypeWordOrTypeExpression()
    ) &&
    thirdToLast.content == "=" && (
      secondToLast.isTypeWordOrTypeExpression() ||
      secondToLast.isIdentifierOrAtom()
    ) &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    this->lastRuleName = "type equals type semicolon";
    fourthToLast.makeFrom3(
      CodeFormatter::Element::Expression,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    this->removeBelowLast(2);
    return true;
  }
  if ((
      fourthToLast.type == CodeFormatter::Element::ClassDeclaration ||
      fourthToLast.isTypeWordOrTypeExpression()
    ) &&
    thirdToLast.content == "=" && (
      secondToLast.isTypeWordOrTypeExpression() ||
      secondToLast.isIdentifierOrAtom()
    )
  ) {
    this->lastRuleName = "type equals type";
    fourthToLast.makeFrom3(
      CodeFormatter::Element::TypeExpression,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    this->removeBelowLast(2);
    return true;
  }
  if (
    thirdToLast.type == CodeFormatter::Element::StructKeyWord &&
    secondToLast.isIdentifierOrAtom() &&
    last.isStarOrAmpersand()
  ) {
    this->lastRuleName = "struct pointer";
    thirdToLast.makeFrom3(
      CodeFormatter::Element::TypeExpression, thirdToLast, secondToLast, last
    );
    return this->removeLast(2);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::StructKeyWord && (
      secondToLast.isIdentifierOrAtom() ||
      secondToLast.isTypeWordOrTypeExpression()
    ) &&
    last.type == CodeFormatter::Element::Atom
  ) {
    this->lastRuleName = "struct atom atom";
    thirdToLast.makeFrom3(
      CodeFormatter::Element::ClassDeclaration, thirdToLast, secondToLast, last
    );
    return this->removeLast(2);
  }
  if ((
      thirdToLast.type == CodeFormatter::Element::ClassKeyWord ||
      thirdToLast.type == CodeFormatter::Element::StructKeyWord
    ) && (
      secondToLast.isIdentifierOrAtom() ||
      secondToLast.type == CodeFormatter::Element::TypeExpression
    ) && (
      last.type == CodeFormatter::Element::SemiColon ||
      last.type == CodeFormatter::Element::LeftCurlyBrace ||
      last.type == CodeFormatter::Element::Colon ||
      last.type == CodeFormatter::Element::GreaterThan ||
      last.content == "="
    )
  ) {
    this->lastRuleName = "class declaration";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::ClassDeclaration, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TypenameKeyWord &&
    secondToLast.isIdentifierOrAtom() &&
    last.content == "="
  ) {
    this->lastRuleName = "typename identifier =";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TypenameKeyWord &&
    secondToLast.isIdentifierOrAtom() && (
      last.type == CodeFormatter::Element::SemiColon ||
      last.type == CodeFormatter::Element::LeftCurlyBrace ||
      last.type == CodeFormatter::Element::GreaterThan
    )
  ) {
    this->lastRuleName = "type declaration";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TypenameKeyWord &&
    secondToLast.isTypeWordOrTypeExpression() &&
    last.isExpressionIdentifierOrAtom()
  ) {
    this->lastRuleName = "typename typeExpression X";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if ((
      thirdToLast.type == CodeFormatter::Element::ClassKeyWord ||
      thirdToLast.type == CodeFormatter::Element::StructKeyWord
    ) &&
    secondToLast.isIdentifierOrAtom() &&
    last.type == CodeFormatter::Element::Comma
  ) {
    this->lastRuleName = "class declaration comma";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::ClassDeclaration, thirdToLast, secondToLast
    );
    thirdToLast.makeFrom1(CodeFormatter::Element::TypeExpression, thirdToLast);
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TypenameKeyWord &&
    secondToLast.isIdentifierOrAtom() &&
    last.type == CodeFormatter::Element::Comma
  ) {
    this->lastRuleName = "typename identifier";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if ((
      fourthToLast.type == CodeFormatter::Element::FunctionDeclaration ||
      fourthToLast.type == CodeFormatter::Element::TypeAndIdentifier
    ) &&
    thirdToLast.isOperator() &&
    thirdToLast.content == "=" && (
      secondToLast.isTypeOrIdentifierOrExpression() ||
      secondToLast.isTypeWordOrTypeExpression()
    ) && (
      last.type == CodeFormatter::Element::Comma ||
      last.type == CodeFormatter::Element::GreaterThan ||
      last.type == CodeFormatter::Element::RightParenthesis ||
      last.type == CodeFormatter::Element::SemiColon
    )
  ) {
    this->lastRuleName = "typeAndIdentifier equals expression";
    fourthToLast.makeFrom3(
      CodeFormatter::Element::Expression,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(2);
  }
  if (
    secondToLast.type == CodeFormatter::Element::TypeAdjectiveKeyWord &&
    last.type == CodeFormatter::Element::EnumKeyWord
  ) {
    this->lastRuleName = "typedef enum";
    secondToLast.makeFrom2(
      CodeFormatter::Element::EnumKeyWord, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    thirdToLast.type == CodeFormatter::Element::EnumKeyWord &&
    secondToLast.isIdentifierOrAtom() && (
      last.type == CodeFormatter::Element::SemiColon ||
      last.type == CodeFormatter::Element::LeftCurlyBrace
    )
  ) {
    this->lastRuleName = "enum declaration";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::EnumDeclaration, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    fourthToLast.type == CodeFormatter::Element::EnumKeyWord && (
      thirdToLast.isExpressionOrAtom() ||
      thirdToLast.type == CodeFormatter::Element::CurlyBraceCommaDelimitedList
    ) &&
    secondToLast.isIdentifierOrAtom() &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    this->lastRuleName = "enum {} typename";
    fourthToLast.makeFrom4(
      CodeFormatter::Element::EnumDefinition,
      fourthToLast,
      thirdToLast,
      secondToLast,
      last
    );
    return this->removeLast(3);
  }
  if ((
      thirdToLast.type == CodeFormatter::Element::ClassDeclaration ||
      thirdToLast.type == CodeFormatter::Element::UnionKeyWord
    ) &&
    secondToLast.type == CodeFormatter::Element::CodeBlock &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    this->lastRuleName = "class definition";
    thirdToLast.makeFrom3(
      CodeFormatter::Element::ClassDefinition, thirdToLast, secondToLast, last
    );
    return this->removeLast(2);
  }
  if (
    fifthToLast.type == CodeFormatter::Element::EnumDeclaration &&
    fourthToLast.type == CodeFormatter::Element::LeftCurlyBrace &&
    thirdToLast.type == CodeFormatter::Element::CommaList &&
    secondToLast.type == CodeFormatter::Element::RightCurlyBrace &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    this->lastRuleName = "enum definition";
    fifthToLast.makeFrom5(
      CodeFormatter::Element::EnumDefinition,
      fifthToLast,
      fourthToLast,
      thirdToLast,
      secondToLast,
      last
    );
    return this->removeLast(4);
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
  if (secondToLast.content == "extern" && last.content == "\"C\"") {
    this->lastRuleName = "extern c";
    secondToLast.makeFrom2(
      CodeFormatter::Element::ControlWantsCodeBlock, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    thirdToLast.isComment() &&
    secondToLast.type == CodeFormatter::Element::TypeAndIdentifier
  ) {
    this->lastRuleName = "comment type and identifier";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeAndIdentifier, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::LeftParenthesis &&
    secondToLast.isComment() &&
    last.type == CodeFormatter::Element::RightParenthesis
  ) {
    this->lastRuleName = "leftParenthesis comment rightParenthesis";
    thirdToLast.makeFrom3(
      CodeFormatter::Element::InParentheses, thirdToLast, secondToLast, last
    );
    return this->removeLast(2);
  }
  if (
    thirdToLast.isExpressionIdentifierAtomOrFunctionWithArguments() &&
    secondToLast.canBeUnaryOnTheRight() && (
      last.isRightDelimiter() ||
      last.type == CodeFormatter::Element::SemiColon ||
      last.type == CodeFormatter::Element::Comma
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
    secondToLast.makeFrom1(CodeFormatter::Element::Expression, secondToLast);
    return true;
  }
  if (
    secondToLast.type == CodeFormatter::Element::FunctionWithArguments && (
      last.type == CodeFormatter::Element::LessThan || (
        last.type == CodeFormatter::Element::GreaterThan &&
        thirdToLast.type != CodeFormatter::Element::LessThan
      )
    )
  ) {
    this->lastRuleName = "function with arguments inequality";
    last.type = CodeFormatter::Element::Operator;
    secondToLast.makeFrom1(CodeFormatter::Element::Expression, secondToLast);
    return true;
  }
  if (
    thirdToLast.content == "." &&
    secondToLast.content == "." &&
    last.content == "."
  ) {
    this->lastRuleName = "three dots";
    thirdToLast.content = "...";
    thirdToLast.makeFrom1(CodeFormatter::Element::Expression, thirdToLast);
    return this->removeLast(2);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::LeftParenthesis &&
    secondToLast.isSuitableForParenthesesEnclosure() &&
    last.type == CodeFormatter::Element::RightParenthesis
  ) {
    this->lastRuleName = "in parentheses";
    thirdToLast.makeFrom3(
      CodeFormatter::Element::InParentheses, thirdToLast, secondToLast, last
    );
    return this->removeLast(2);
  }
  if (
    fourthToLast.type == CodeFormatter::Element::LeftParenthesis &&
    thirdToLast.isSuitableForParenthesesEnclosure() &&
    secondToLast.type == CodeFormatter::Element::RightParenthesis
  ) {
    this->lastRuleName = "in parentheses";
    fourthToLast.makeFrom3(
      CodeFormatter::Element::InParentheses,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(2);
  }
  if (
    fourthToLast.type != CodeFormatter::Element::EnumDeclaration &&
    thirdToLast.type == CodeFormatter::Element::LeftCurlyBrace && (
      secondToLast.type == CodeFormatter::Element::CommaList ||
      secondToLast.isExpressionIdentifierAtomOrFunctionWithArguments()
    ) &&
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
    fourthToLast.type == CodeFormatter::Element::NewKeyWord && (
      thirdToLast.isIdentifierOrAtom() ||
      thirdToLast.isTypeWordOrTypeExpression()
    ) &&
    secondToLast.type == CodeFormatter::Element::Star &&
    last.type == CodeFormatter::Element::LeftBracket
  ) {
    this->lastRuleName = "new element star leftBracket";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if ((
      thirdToLast.type == CodeFormatter::Element::DeleteKeyWord ||
      thirdToLast.type == CodeFormatter::Element::NewKeyWord
    ) && (
      secondToLast.isTypeOrIdentifierOrExpression() ||
      secondToLast.type == CodeFormatter::Element::TypeExpression
    ) && (last.isRightDelimiterOrSemicolon() || last.isLeftDelimiter())
  ) {
    this->lastRuleName = "new|delete Expression X";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::Expression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    fourthToLast.type == CodeFormatter::Element::DoKeyWord &&
    thirdToLast.type == CodeFormatter::Element::CodeBlock &&
    secondToLast.type == CodeFormatter::Element::ControlWantsCodeBlock &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    this->lastRuleName = "do {} while;";
    fourthToLast.makeFrom4(
      CodeFormatter::Element::Command,
      fourthToLast,
      thirdToLast,
      secondToLast,
      last
    );
    return this->removeLast(3);
  }
  if (
    fifthToLast.type == CodeFormatter::Element::DeleteKeyWord &&
    fourthToLast.type == CodeFormatter::Element::LeftBracket &&
    thirdToLast.type == CodeFormatter::Element::RightBracket && (
      secondToLast.isTypeWordOrTypeExpression() ||
      secondToLast.isExpressionIdentifierOrAtom()
    ) &&
    last.isRightDelimiterOrSemicolon()
  ) {
    this->lastRuleName = "delete leftBracket rightBracket Expression X";
    fifthToLast.makeFrom4(
      CodeFormatter::Element::Expression,
      fifthToLast,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(3);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::LeftBracket &&
    secondToLast.isExpressionIdentifierAtomFunctionWithArgumentsOrInParentheses
    () &&
    last.type == CodeFormatter::Element::RightBracket
  ) {
    this->lastRuleName = "leftBracket expression rightBracket";
    thirdToLast.makeFrom3(
      CodeFormatter::Element::InBrackets, thirdToLast, secondToLast, last
    );
    return this->removeLast(2);
  }
  if ((
      thirdToLast.isTypeWordOrTypeExpression() ||
      thirdToLast.isIdentifierOrAtom()
    ) &&
    secondToLast.isExpressionIdentifierOrAtom() &&
    last.type == CodeFormatter::Element::InBrackets
  ) {
    this->lastRuleName = "typeExpression identifier inBrackets";
    thirdToLast.makeFrom3(
      CodeFormatter::Element::TypeAndIdentifier,
      thirdToLast,
      secondToLast,
      last
    );
    return this->removeLast(2);
  }
  if ((
      secondToLast.
      isExpressionIdentifierAtomFunctionWithArgumentsOrInParentheses() ||
      secondToLast.type == CodeFormatter::Element::TypeAndIdentifier
    ) &&
    last.type == CodeFormatter::Element::InBrackets
  ) {
    this->lastRuleName = "expression followed by brackets";
    secondToLast.makeFrom2(
      CodeFormatter::Element::Expression, secondToLast, last
    );
    return this->removeLast();
  }
  if ((
      fourthToLast.
      isExpressionIdentifierAtomFunctionWithArgumentsOrInParentheses() ||
      fourthToLast.isTypeWordOrTypeExpression()
    ) &&
    thirdToLast.type == CodeFormatter::Element::LeftBracket &&
    secondToLast.isExpressionIdentifierOrAtom() &&
    last.type == CodeFormatter::Element::RightBracket
  ) {
    this->lastRuleName = "something leftBracket expression rightBracket";
    CodeFormatter::Element element;
    element.makeFrom3(
      CodeFormatter::Element::InBrackets, thirdToLast, secondToLast, last
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
    this->lastRuleName = "combine operators";
    secondToLast.content = secondToLast.content + last.content;
    secondToLast.type = CodeFormatter::Element::Operator;
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::FunctionWithArguments &&
    last.type == CodeFormatter::Element::ConstKeyWord
  ) {
    this->lastRuleName = "function with arguments const";
    secondToLast.makeFrom2(
      CodeFormatter::Element::FunctionDeclaration, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    thirdToLast.type == CodeFormatter::Element::OperatorKeyWord && (
      secondToLast.isOperatorOrInequality() || secondToLast.content == "bool"
    ) && (
      last.type == CodeFormatter::Element::LeftParenthesis ||
      last.type == CodeFormatter::Element::LessThan
    )
  ) {
    this->lastRuleName = "operatorKeyWord operator";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::Identifier, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    secondToLast.type == CodeFormatter::Element::FunctionWithArguments &&
    last.type == CodeFormatter::Element::Operator
  ) {
    this->lastRuleName = "function with arguments to expression";
    secondToLast.makeFrom1(CodeFormatter::Element::Expression, secondToLast);
    return true;
  }
  if ((
      thirdToLast.type == CodeFormatter::Element::ConstKeyWord ||
      thirdToLast.type == CodeFormatter::Element::TypeAdjectiveKeyWord ||
      thirdToLast.type == CodeFormatter::Element::FriendKeyWord
    ) && (
      secondToLast.isExpressionIdentifierOrAtom() ||
      secondToLast.type == CodeFormatter::Element::TypeExpression
    ) &&
    last.type != CodeFormatter::Element::Colon &&
    last.type != CodeFormatter::Element::DoubleColon &&
    last.type != CodeFormatter::Element::LessThan
  ) {
    this->lastRuleName = "type adjective something";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    secondToLast.type == CodeFormatter::Element::TemplateClause &&
    last.isComment()
  ) {
    this->lastRuleName = "template clause and comment";
    secondToLast.addChild(last);
    return this->removeLast();
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TemplateKeyWord &&
    secondToLast.type == CodeFormatter::Element::LessThan &&
    last.type == CodeFormatter::Element::GreaterThan
  ) {
    this->lastRuleName = "template lessThan greaterThan";
    CodeFormatter::Element inAngleBrackets;
    inAngleBrackets.makeFrom2(
      CodeFormatter::Element::InAngleBrackets, secondToLast, last
    );
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TemplateClause, thirdToLast, inAngleBrackets
    );
    return this->removeLast(2);
  }
  if (
    fourthToLast.type == CodeFormatter::Element::TemplateKeyWord &&
    thirdToLast.type == CodeFormatter::Element::LessThan &&
    secondToLast.isSuitableForTemplateArgument() &&
    last.type == CodeFormatter::Element::GreaterThan
  ) {
    this->lastRuleName = "template lessThan typeExpression greaterThan";
    CodeFormatter::Element inAngleBrackets;
    inAngleBrackets.makeFrom3(
      CodeFormatter::Element::InAngleBrackets, thirdToLast, secondToLast, last
    );
    fourthToLast.makeFrom2(
      CodeFormatter::Element::TemplateClause, fourthToLast, inAngleBrackets
    );
    return this->removeLast(3);
  }
  if (
    fourthToLast.isSuitableForTypeExpression() &&
    thirdToLast.type == CodeFormatter::Element::LessThan &&
    secondToLast.isSuitableForTemplateArgument() &&
    last.type == CodeFormatter::Element::GreaterThan
  ) {
    this->lastRuleName =
    "type expression less than type expression greater than";
    CodeFormatter::Element inAngleBrackets;
    inAngleBrackets.makeFrom3(
      CodeFormatter::Element::InAngleBrackets, thirdToLast, secondToLast, last
    );
    fourthToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, fourthToLast, inAngleBrackets
    );
    return this->removeLast(3);
  }
  if (
    fifthToLast.isSuitableForTypeExpression() &&
    fourthToLast.type == CodeFormatter::Element::LessThan &&
    thirdToLast.isSuitableForTemplateArgument() &&
    secondToLast.type == CodeFormatter::Element::GreaterThan
  ) {
    this->lastRuleName =
    "type expression less than type expression greater than X";
    CodeFormatter::Element inAngleBrackets;
    inAngleBrackets.makeFrom3(
      CodeFormatter::Element::InAngleBrackets,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    fifthToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, fifthToLast, inAngleBrackets
    );
    return this->removeBelowLast(3);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TypeExpression &&
    secondToLast.isStarOrAmpersand()
  ) {
    this->lastRuleName = "absorb star or ampersand";
    thirdToLast.addChild(secondToLast);
    return this->removeBelowLast(1);
  }
  if (
    secondToLast.type == CodeFormatter::Element::TypeExpression &&
    last.type == CodeFormatter::Element::TypeKeyWord
  ) {
    this->lastRuleName = "type expression type key word";
    secondToLast.addChild(last);
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
    this->lastRuleName = "type and type to typeExpression";
    secondToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, secondToLast, last
    );
    return this->removeLast();
  }
  if ((
      secondToLast.type == CodeFormatter::Element::ConstKeyWord ||
      secondToLast.type == CodeFormatter::Element::TypeAdjectiveKeyWord ||
      secondToLast.type == CodeFormatter::Element::FriendKeyWord
    ) &&
    last.isTypeWordOrTypeExpression()
  ) {
    this->lastRuleName = "typeKeyWord and typeExpression to typeExpression";
    secondToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    thirdToLast.isSuitableForTypeExpression() &&
    this->allowTypeAndIdentifier(fourthToLast, secondToLast, last)
  ) {
    this->lastRuleName = "type and identifier";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeAndIdentifier, thirdToLast, secondToLast
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
      CodeFormatter::Element::TypeAndIdentifier, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (this->isSuitableForFunction(secondToLast) && last.isParenthesesBlock()) {
    this->lastRuleName = "function of arguments";
    secondToLast.makeFrom2(
      CodeFormatter::Element::FunctionWithArguments, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    this->isSuitableForFunction(thirdToLast) &&
    secondToLast.isParenthesesBlock()
  ) {
    this->lastRuleName = "function of arguments";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::FunctionWithArguments, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    this->isSuitableForExpressionOperatorExpression(
      sixthToLast, fifthToLast, fourthToLast, thirdToLast
    )
  ) {
    // Must come before rule
    // expression oeprator expression Lookahead X.
    this->lastRuleName = "expression operator expression Lookahead XX";
    fifthToLast.type = CodeFormatter::Element::Operator;
    sixthToLast.makeFrom3(
      CodeFormatter::Element::Expression,
      sixthToLast,
      fifthToLast,
      fourthToLast
    );
    this->stack.removeIndexShiftDown(this->stack.size - 5);
    this->stack.removeIndexShiftDown(this->stack.size - 5);
    return true;
  }
  if (
    this->isSuitableForTypePointer(
      fifthToLast, fourthToLast, thirdToLast, secondToLast, last
    )
  ) {
    // Must come before rule expression operator expression.
    this->lastRuleName = "type and ampersand or star";
    fourthToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, fourthToLast, thirdToLast
    );
    this->stack.removeIndexShiftDown(this->stack.size - 3);
    return true;
  }
  if (
    sixthToLast.type == CodeFormatter::Element::LeftParenthesis && (
      fifthToLast.isExpressionOrAtom() ||
      fifthToLast.isIdentifierOrAtom() ||
      fifthToLast.isTypeWordOrTypeExpression()
    ) &&
    fourthToLast.isStarOrAmpersand() &&
    thirdToLast.type == CodeFormatter::Element::Atom &&
    secondToLast.type == CodeFormatter::Element::Colon
  ) {
    this->lastRuleName = "(expression& atom:X";
    fifthToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, fifthToLast, fourthToLast
    );
    this->stack.removeIndexShiftDown(this->stack.size - 4);
    return true;
  }
  if (
    this->isSuitableForExpressionOperatorExpressionXX(
      fifthToLast, fourthToLast, thirdToLast, secondToLast, last
    )
  ) {
    // Must come before rule
    // expression operator expression Lookahead.
    this->lastRuleName = "expression operator expression Lookahead X";
    fourthToLast.type = CodeFormatter::Element::Operator;
    fifthToLast.makeFrom3(
      CodeFormatter::Element::Expression,
      fifthToLast,
      fourthToLast,
      thirdToLast
    );
    this->stack.removeIndexShiftDown(this->stack.size - 4);
    this->stack.removeIndexShiftDown(this->stack.size - 3);
    return true;
  }
  if (
    this->isSuitableForExpressionOperatorExpression(
      fourthToLast, thirdToLast, secondToLast, last
    )
  ) {
    // This rule must come after the rule
    // expression operator expression Lookahead X
    this->lastRuleName = "expression operator expression";
    // Operators such as < and > may need to have their type
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
    sixthToLast.isExpressionIdentifierAtomFunctionWithArgumentsOrInParentheses(
    ) &&
    fifthToLast.type == CodeFormatter::Element::QuestionMark &&
    fourthToLast.isExpressionIdentifierAtomFunctionWithArgumentsOrInParentheses
    () &&
    thirdToLast.type == CodeFormatter::Element::Colon &&
    secondToLast.isExpressionIdentifierAtomFunctionWithArgumentsOrInParentheses
    () && (
      last.isRightDelimiter() || last.type == CodeFormatter::Element::SemiColon
    )
  ) {
    this->lastRuleName = "ternary question mark";
    fifthToLast.type = CodeFormatter::Element::Operator;
    thirdToLast.type = CodeFormatter::Element::Operator;
    sixthToLast.makeFrom5(
      CodeFormatter::Element::Expression,
      sixthToLast,
      fifthToLast,
      fourthToLast,
      thirdToLast,
      secondToLast
    );
    return this->removeBelowLast(4);
  }
  if (
    thirdToLast.isTypeWordOrTypeExpression() &&
    secondToLast.isStarOrAmpersand() &&
    last.type == CodeFormatter::Element::FunctionWithArguments
  ) {
    this->lastRuleName = "typeExpression star";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.isTypeWordOrTypeExpression() &&
    secondToLast.type == CodeFormatter::Element::FunctionWithArguments
  ) {
    this->lastRuleName = "typeExpression functionWithArguments";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::FunctionDeclaration, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TypeKeyWord &&
    secondToLast.isStarOrAmpersand() && (
      last.type == CodeFormatter::Element::Comma ||
      last.type == CodeFormatter::Element::RightParenthesis
    )
  ) {
    this->lastRuleName = "type and ampersand or star comma";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::TypeExpression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    this->isSuitableForUnaryOperatorExpression(
      fourthToLast, thirdToLast, secondToLast, last
    )
  ) {
    this->lastRuleName = "X unary expression";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::Expression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    this->isSuitableForUnaryOperatorExpressionXX(
      fifthToLast, fourthToLast, thirdToLast, secondToLast
    )
  ) {
    this->lastRuleName = "X unary expression YZ";
    fourthToLast.makeFrom2(
      CodeFormatter::Element::Expression, fourthToLast, thirdToLast
    );
    this->stack.removeIndexShiftDown(this->stack.size - 3);
    return true;
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
  if (
    this->isSuitableForExpressionExpressionToExpression(
      thirdToLast, secondToLast
    ) &&
    !last.isColonOrDoubleColon() &&
    last.type != CodeFormatter::Element::LessThan
  ) {
    this->lastRuleName = "expression and atom";
    thirdToLast.appendExpression(secondToLast);
    return this->removeBelowLast(1);
  }
  if (
    secondToLast.isExpressionIdentifierAtomOrFunctionWithArguments() &&
    last.isComment()
  ) {
    this->lastRuleName = "expression and comment";
    secondToLast.appendType(last, CodeFormatter::Element::Expression);
    return this->removeLast();
  }
  if (thirdToLast.isParenthesesBlock() && secondToLast.isExpressionOrAtom()) {
    this->lastRuleName = "in parentheses and expression";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::Expression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if (
    secondToLast.type == CodeFormatter::Element::CommaList && last.isComment()
  ) {
    this->lastRuleName = "comma list and comment";
    secondToLast.addChild(last);
    return this->removeLast();
  }
  if (
    thirdToLast.type == CodeFormatter::Element::CommaList &&
    this->isSuitableForCommaSeparatedList(secondToLast) &&
    last.type == CodeFormatter::Element::Comma
  ) {
    this->lastRuleName = "comma list and element";
    thirdToLast.addChild(secondToLast);
    thirdToLast.addChild(last);
    return this->removeLast(2);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::CommaList &&
    this->isSuitableForCommaSeparatedList(secondToLast) && (
      last.isRightDelimiter() ||
      last.type == CodeFormatter::Element::SemiColon ||
      last.type == CodeFormatter::Element::GreaterThan
    )
  ) {
    this->lastRuleName = "comma list item delimiter";
    thirdToLast.addChild(secondToLast);
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
    this->areSuitableForCommaSeparatedList(fourthToLast, secondToLast) &&
    thirdToLast.type == CodeFormatter::Element::Comma &&
    last.type == CodeFormatter::Element::Comma
  ) {
    this->lastRuleName = "expression comma expression";
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
      secondToLast.type == CodeFormatter::Element::TypeAndIdentifier ||
      secondToLast.
      isExpressionIdentifierAtomFunctionWithArgumentsOrInParentheses()
    ) &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    this->lastRuleName = "return expression semicolon";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::ReturnedExpression, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
  }
  if ((
      fourthToLast.type == CodeFormatter::Element::ClassDeclaration ||
      fourthToLast.type == CodeFormatter::Element::UnionKeyWord
    ) &&
    thirdToLast.type == CodeFormatter::Element::CodeBlock && (
      secondToLast.type == CodeFormatter::Element::Atom ||
      secondToLast.type == CodeFormatter::Element::CommaList
    ) &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    this->lastRuleName = "class definition";
    fourthToLast.makeFrom4(
      CodeFormatter::Element::ClassDefinition,
      fourthToLast,
      thirdToLast,
      secondToLast,
      last
    );
    return this->removeLast(3);
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
    secondToLast.makeFrom1(CodeFormatter::Element::Expression, secondToLast);
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
  if ((
      secondToLast.type == CodeFormatter::Element::LeftParenthesis ||
      secondToLast.type == CodeFormatter::Element::LeftCurlyBrace
    ) &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    this->lastRuleName = "left delimiter semicolon";
    last.makeFrom1(CodeFormatter::Element::Command, last);
    return true;
  }
  if (
    thirdToLast.type == CodeFormatter::Element::TypeAndIdentifier &&
    secondToLast.isParenthesesBlock()
  ) {
    // Must come before expression and semicolon.
    this->lastRuleName = "type_identifier + parentheses block X";
    thirdToLast.makeFrom2(
      CodeFormatter::Element::FunctionDeclaration, thirdToLast, secondToLast
    );
    return this->removeBelowLast(1);
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
    this->isSuitableForCommaSeparatedList(secondToLast) &&
    last.type == CodeFormatter::Element::Comma
  ) {
    this->lastRuleName = "expression comma";
    secondToLast.makeFrom2(
      CodeFormatter::Element::CommaList, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    secondToLast.type == secondToLast.TypeExpression &&
    last.type == CodeFormatter::Element::Comma
  ) {
    this->lastRuleName = "type expression comma";
    secondToLast.makeFrom2(
      CodeFormatter::Element::CommaList, secondToLast, last
    );
    return this->removeLast();
  }
  if (
    thirdToLast.type != CodeFormatter::Element::EnumDeclaration &&
    secondToLast.type == CodeFormatter::Element::LeftCurlyBrace && (
      last.type == CodeFormatter::Element::VisibilityClause ||
      last.type == CodeFormatter::Element::Command ||
      last.type == CodeFormatter::Element::EnumDefinition ||
      last.type == CodeFormatter::Element::ClassDefinition
    )
  ) {
    this->lastRuleName = "leftCurlyBrace command to commandList";
    last.makeFrom1(CodeFormatter::Element::CommandList, last);
    return true;
  }
  if (
    thirdToLast.type == CodeFormatter::Element::LeftCurlyBrace && (
      secondToLast.isComment() ||
      secondToLast.type == CodeFormatter::Element::MacroLine
    ) && (
      last.type == CodeFormatter::Element::CommandList ||
      last.type == CodeFormatter::Element::VisibilityClause ||
      last.type == CodeFormatter::Element::Command ||
      last.type == CodeFormatter::Element::EnumDefinition ||
      last.type == CodeFormatter::Element::ClassDeclaration ||
      last.type == CodeFormatter::Element::ClassDefinition ||
      last.type == CodeFormatter::Element::If
    )
  ) {
    this->lastRuleName = "leftCurlyBrace comment|macro command list";
    secondToLast.makeFrom1(CodeFormatter::Element::CommandList, secondToLast);
    return true;
  }
  if (
    secondToLast.isComment() && last.type == CodeFormatter::Element::CommaList
  ) {
    this->lastRuleName = "comment comma list";
    secondToLast.makeFrom1(CodeFormatter::Element::CommaList, secondToLast);
    secondToLast.children.addListOnTop(last.children);
    return this->removeLast();
  }
  if (
    thirdToLast.type == CodeFormatter::Element::LeftCurlyBrace && (
      secondToLast.isComment() ||
      secondToLast.type == CodeFormatter::Element::MacroLine
    ) &&
    last.type == CodeFormatter::Element::CommaList
  ) {
    this->lastRuleName = "leftCurlyBrace comment|macro comma list";
    secondToLast.makeFrom1(CodeFormatter::Element::CommandList, secondToLast);
    secondToLast.children.addListOnTop(last.children);
    return this->removeLast();
  }
  if (
    thirdToLast.type == CodeFormatter::Element::LeftCurlyBrace && (
      secondToLast.isComment() ||
      secondToLast.type == CodeFormatter::Element::MacroLine
    ) && (
      last.type == CodeFormatter::Element::CaseClause ||
      last.type == CodeFormatter::Element::CaseKeyWord ||
      last.type == CodeFormatter::Element::CaseClauseMultipleStart ||
      last.type == CodeFormatter::Element::CaseClauseStart ||
      last.type == CodeFormatter::Element::CaseClauseList
    )
  ) {
    this->lastRuleName = "leftCurlyBrace comment|macro case clause list";
    secondToLast.makeFrom1(
      CodeFormatter::Element::CaseClauseList, secondToLast
    );
    return true;
  }
  if (
    thirdToLast.isCommandListOrCommand() &&
    secondToLast.type == CodeFormatter::Element::MacroLine
  ) {
    this->lastRuleName = "commandList macroLine";
    thirdToLast.appendType(secondToLast, CodeFormatter::Element::CommandList);
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.isCommandListOrCommand() &&
    secondToLast.type == CodeFormatter::Element::Command
  ) {
    this->lastRuleName = "commandList command something";
    thirdToLast.appendType(secondToLast, CodeFormatter::Element::CommandList);
    return this->removeBelowLast(1);
  }
  if (
    secondToLast.isCommandListOrCommand() &&
    last.type == CodeFormatter::Element::SemiColon
  ) {
    this->lastRuleName = "single semicolon to expression";
    last.type = CodeFormatter::Element::Command;
    return true;
  }
  if ((
      secondToLast.isCommandListOrCommand() ||
      secondToLast.type == CodeFormatter::Element::VisibilityClause
    ) && (
      last.type == CodeFormatter::Element::Command ||
      last.type == CodeFormatter::Element::Comment ||
      last.type == CodeFormatter::Element::CommentMultiline ||
      last.type == CodeFormatter::Element::VisibilityClause ||
      last.type == CodeFormatter::Element::EnumDefinition ||
      last.type == CodeFormatter::Element::ClassDefinition ||
      last.type == CodeFormatter::Element::CodeBlock
    )
  ) {
    this->lastRuleName =
    "commandList command|comment|clause|definition|codeblock";
    secondToLast.appendType(last, CodeFormatter::Element::CommandList);
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
    secondToLast.appendType(last, CodeFormatter::Element::CommandList);
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
    secondToLast.appendType(last, CodeFormatter::Element::CommandList);
    return this->removeLast();
  }
  if (
    thirdToLast.isCommandListOrCommand() && (
      secondToLast.isExpressionIdentifierOrAtom() ||
      secondToLast.type == CodeFormatter::Element::CommaList
    ) &&
    last.type == CodeFormatter::Element::RightParenthesis
  ) {
    this->lastRuleName = "commandList expression)";
    thirdToLast.appendType(secondToLast, CodeFormatter::Element::CommandList);
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::LeftCurlyBrace && (
      secondToLast.isCommandListOrCommand() ||
      secondToLast.type == CodeFormatter::Element::CaseClauseList ||
      secondToLast.isComment() ||
      secondToLast.type == CodeFormatter::Element::ClassDefinition ||
      secondToLast.type == CodeFormatter::Element::ClassDeclaration
    ) &&
    last.type == CodeFormatter::Element::RightCurlyBrace
  ) {
    this->lastRuleName = "curly brace case clause list";
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
    thirdToLast.type == CodeFormatter::Element::FunctionDeclaration &&
    secondToLast.type == CodeFormatter::Element::Colon &&
    last.type == CodeFormatter::Element::FunctionWithArguments
  ) {
    this->lastRuleName = "functionDeclaration colon functionWithArguments";
    thirdToLast.makeFrom3(
      CodeFormatter::Element::FunctionDeclaration,
      thirdToLast,
      secondToLast,
      last
    );
    return this->removeLast(2);
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
    this->lastRuleName = "atom inParentheses codeBlock";
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
  if (last.type == CodeFormatter::Element::EndFile) {
    this->lastRuleName = "remove endFile";
    return this->removeLast();
  }
  return false;
}

bool CodeFormatter::Processor::allowTypeAndIdentifier(
  CodeFormatter::Element& prefix,
  CodeFormatter::Element& toBeTyped,
  CodeFormatter::Element& suffix
) {
  if (
    !toBeTyped.isIdentifierOrAtom() &&
    toBeTyped.type != CodeFormatter::Element::TypeExpression
  ) {
    return false;
  }
  if (suffix.isColonOrDoubleColon()) {
    return false;
  }
  if (suffix.type == CodeFormatter::Element::LessThan) {
    // We have an expression along the lines of:
    // bool Vectors<
    // This likely extends along the lines of:
    // bool Vectors<Rational>()
    return false;
  }
  if (suffix.type == CodeFormatter::Element::LeftBracket) {
    // Example: z*w[2]
    return false;
  }
  if ((
      prefix.type == CodeFormatter::Element::LeftCurlyBrace ||
      prefix.isCommandListOrCommand()
    ) &&
    suffix.type == CodeFormatter::Element::Comma
  ) {
    return false;
  }
  return true;
}

bool CodeFormatter::Processor::isSuitableForTypePointer(
  CodeFormatter::Element& prefix,
  CodeFormatter::Element& type,
  CodeFormatter::Element& starOrAmpersand,
  CodeFormatter::Element& suffix,
  Element& afterSuffix
) {
  if (
    starOrAmpersand.type != CodeFormatter::Element::Ampersand &&
    starOrAmpersand.type != CodeFormatter::Element::Star
  ) {
    return false;
  }
  if (!type.isIdentifierOrAtom() && !type.isTypeWordOrTypeExpression()) {
    return false;
  }
  if (
    prefix.type == CodeFormatter::Element::LessThan &&
    suffix.type == CodeFormatter::Element::GreaterThan
  ) {
    return true;
  }
  if (
    prefix.type == CodeFormatter::Element::LeftParenthesis &&
    afterSuffix.type == CodeFormatter::Element::LeftParenthesis
  ) {
    return false;
  }
  if (
    prefix.type != CodeFormatter::Element::CommaList &&
    prefix.type != CodeFormatter::Element::CommandList &&
    prefix.type != CodeFormatter::Element::Comma &&
    prefix.type != CodeFormatter::Element::LeftParenthesis &&
    prefix.type != CodeFormatter::Element::LeftCurlyBrace &&
    prefix.type != CodeFormatter::Element::TopLevel &&
    prefix.type != CodeFormatter::Element::TemplateClause
  ) {
    return false;
  }
  if (suffix.type == CodeFormatter::Element::RightParenthesis) {
    return true;
  }
  if (suffix.isStarOrAmpersand()) {
    return true;
  }
  if (!suffix.isIdentifierOrAtom()) {
    return false;
  }
  if ((
      prefix.type == CodeFormatter::Element::LeftCurlyBrace ||
      prefix.type == CodeFormatter::Element::CommandList ||
      prefix.type == CodeFormatter::Element::LeftParenthesis ||
      prefix.type == CodeFormatter::Element::CommaList
    ) &&
    afterSuffix.content == "="
  ) {
    return true;
  }
  if (
    afterSuffix.type != CodeFormatter::Element::Comma &&
    afterSuffix.type != CodeFormatter::Element::RightParenthesis &&
    afterSuffix.type != CodeFormatter::Element::LeftParenthesis &&
    afterSuffix.type != CodeFormatter::Element::SemiColon
  ) {
    return false;
  }
  return true;
}

bool CodeFormatter::Processor::isSuitableForExpressionExpressionToExpression(
  CodeFormatter::Element& left, CodeFormatter::Element& right
) const {
  return
  left.type == CodeFormatter::Element::Expression &&
  right.isIdentifierOrAtom();
}

bool CodeFormatter::Processor::isSuitableForUnaryOperatorExpression(
  CodeFormatter::Element& first,
  CodeFormatter::Element& unary,
  CodeFormatter::Element& expression,
  CodeFormatter::Element& lookAhead
) {
  if (
    !this->isSuitableForUnaryOperatorExpressionXX(
      first, unary, expression, lookAhead
    )
  ) {
    return false;
  }
  if (
    lookAhead.type == CodeFormatter::Element::Colon ||
    lookAhead.type == CodeFormatter::Element::DoubleColon
  ) {
    return false;
  }
  return true;
}

bool CodeFormatter::Processor::isSuitableForUnaryOperatorExpressionXX(
  CodeFormatter::Element& first,
  CodeFormatter::Element& unary,
  CodeFormatter::Element& expression,
  CodeFormatter::Element& lookAhead
) {
  bool canBeUnary = unary.canBeUnaryOnTheLeft();
  if (
    expression.type == CodeFormatter::Element::TypeExpression &&
    lookAhead.isRightDelimiter() &&
    canBeUnary
  ) {
    return true;
  }
  if (!expression.isExpressionIdentifierAtomOrFunctionWithArguments()) {
    return false;
  }
  if (!canBeUnary) {
    return false;
  }
  if (
    lookAhead.type == CodeFormatter::Element::LeftParenthesis ||
    lookAhead.type == CodeFormatter::Element::LeftBracket
  ) {
    // We can have an expression of the form a*b() where
    // the left parenthesis signifies b is a function.
    return false;
  }
  if (lookAhead.type == CodeFormatter::Element::LessThan) {
    // An expression along the lines of:
    // !Matrix<
    // The inequality can be extended to a template: !Matrix<a>.
    return false;
  }
  if (lookAhead.type == CodeFormatter::Element::DoubleColon) {
    return false;
  }
  if (lookAhead.isOperator()) {
    if (this->owner->rightOperatorOverridesLeft(unary, lookAhead)) {
      return false;
    }
  }
  if (first.isIdentifierOrAtom()) {
    return false;
  }
  return true;
}

bool CodeFormatter::Processor::areSuitableForCommaSeparatedList(
  CodeFormatter::Element& left, CodeFormatter::Element& right
) {
  return
  this->isSuitableForCommaSeparatedList(left) &&
  this->isSuitableForCommaSeparatedList(right);
}

bool CodeFormatter::Processor::isNoReturn(CodeFormatter::Element& input) {
  if (input.content == "[noreturn]") {
    return false;
  }
  if (input.children.size != 3) {
    return false;
  }
  return
  input.children[0].content == "[" &&
  input.children[1].content == "noreturn" &&
  input.children[2].content == "]";
}

bool CodeFormatter::Processor::isSuitableForCommaSeparatedList(
  CodeFormatter::Element& input
) {
  if (input.isExpressionIdentifierAtomOrFunctionWithArguments()) {
    return true;
  }
  if (input.type == CodeFormatter::Element::TypeAndIdentifier) {
    return true;
  }
  if (input.type == CodeFormatter::Element::Expression) {
    return true;
  }
  if (
    input.type == CodeFormatter::Element::TypeKeyWord ||
    input.type == CodeFormatter::Element::TypeExpression
  ) {
    return true;
  }
  if (
    input.type == CodeFormatter::Element::ClassDeclaration ||
    input.type == CodeFormatter::Element::FunctionDeclaration
  ) {
    return true;
  }
  if (
    input.type == CodeFormatter::Element::CodeBlock ||
    input.type == CodeFormatter::Element::InParentheses
  ) {
    return true;
  }
  return false;
}

bool CodeFormatter::Processor::isSuitableForFunction(
  CodeFormatter::Element& input
) {
  if (input.isTypeOrIdentifierOrExpression()) {
    return true;
  }
  if (input.type == CodeFormatter::Element::Expression) {
    return true;
  }
  if (input.type == CodeFormatter::Element::InParentheses) {
    return true;
  }
  if (input.type == CodeFormatter::Element::FunctionWithArguments) {
    // A function with argument can return a function pointer.
    return true;
  }
  return false;
}

bool CodeFormatter::Processor::isSuitableForExpressionOperatorExpression(
  CodeFormatter::Element& left,
  CodeFormatter::Element& operatorElement,
  CodeFormatter::Element& right,
  Element& lookAhead
) {
  if (
    !isSuitableForExpressionOperatorExpressionXX(
      left, operatorElement, right, lookAhead, this->dummy
    )
  ) {
    return false;
  }
  if (lookAhead.isColonOrDoubleColon()) {
    return false;
  }
  return true;
}

bool CodeFormatter::Processor::isSuitableForExpressionOperatorExpressionXX(
  CodeFormatter::Element& left,
  CodeFormatter::Element& operatorElement,
  CodeFormatter::Element& right,
  CodeFormatter::Element& lookAheadFirst,
  CodeFormatter::Element& lookAheadNext
) {
  if (
    left.type == CodeFormatter::Element::InParentheses &&
    operatorElement.type == CodeFormatter::Element::LessThan &&
    right.isExpressionIdentifierOrAtom() &&
    lookAheadFirst.type == CodeFormatter::Element::RightParenthesis
  ) {
    return true;
  }
  if (
    !left.isExpressionIdentifierAtomOrFunctionWithArguments() &&
    left.type != CodeFormatter::Element::InParentheses &&
    left.type != CodeFormatter::Element::TypeAndIdentifier &&
    left.type != CodeFormatter::Element::TypeExpression
  ) {
    return false;
  }
  if (
    !right.isExpressionIdentifierAtomOrFunctionWithArguments() &&
    right.type != CodeFormatter::Element::InParentheses &&
    right.type != CodeFormatter::Element::TypeAndIdentifier &&
    right.type != CodeFormatter::Element::TypeExpression
  ) {
    return false;
  }
  if (!operatorElement.isOperatorOrInequality()) {
    return false;
  }
  if (operatorElement.content != "->" && operatorElement.content != ".") {
    if (
      lookAheadFirst.content == "-" ||
      lookAheadFirst.type == CodeFormatter::Element::LeftParenthesis ||
      lookAheadFirst.type == CodeFormatter::Element::LeftBracket
    ) {
      return false;
    }
  }
  if (
    this->owner->rightOperatorOverridesLeft(operatorElement, lookAheadFirst)
  ) {
    return false;
  }
  if (
    operatorElement.type == CodeFormatter::Element::LessThan && (
      lookAheadFirst.type == CodeFormatter::Element::Comma ||
      lookAheadFirst.type == CodeFormatter::Element::GreaterThan
    )
  ) {
    // This could be something of the form:
    // List<a,
    // which can extend to a template: List<a,b>;
    // or of the form:
    // List<a>.
    return false;
  }
  if (lookAheadFirst.type == CodeFormatter::Element::QuestionMark) {
    return true;
  }
  if (lookAheadFirst.type == CodeFormatter::Element::LessThan) {
    return false;
  }
  if (
    lookAheadFirst.type == CodeFormatter::Element::DoubleColon &&
    lookAheadNext.type == CodeFormatter::Element::OperatorKeyWord
  ) {
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
  this->debugLog +=
  this->toStringStack() +
  "<b style='color:red'>[[" +
  this->lastRuleName +
  "]]</b><br>";
}

std::string CodeFormatter::Processor::toStringStack() {
  std::stringstream out;
  for (int i = this->owner->dummyElements; i < this->stack.size; i ++) {
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

bool CodeFormatter::writeFormattedCode(std::stringstream* comments) {
  STACK_TRACE("CodeFormatter::writeFormattedCode");
  std::string filenameOut = this->outputFileName;
  if (!this->parsingSucceeded()) {
    filenameOut = this->outputOnFailFileName;
  }
  if (
    !FileOperations::writeFileVirtual(
      filenameOut, this->transformedContent, comments
    )
  ) {
    if (comments != nullptr) {
      *comments << "Failed to open source code formatting output file. ";
    }
    return false;
  }
  return true;
}

bool CodeFormatter::rightOperatorOverridesLeft(
  const CodeFormatter::Element& leftOperator,
  const CodeFormatter::Element& rightOperator
) {
  if (
    !rightOperator.isOperatorOrInequality() &&
    !rightOperator.isStarOrAmpersand() &&
    rightOperator.type != CodeFormatter::Element::QuestionMark
  ) {
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
  CodeFormatter::Element & middle = inputOutput.children[1];
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
  CodeFormatter::Element & middle = current.children[1];
  if (!middle.isOperator() || middle.content != operatorName) {
    output.addOnTop(current);
    return;
  }
  this->collectMultiArguments(operatorName, current.children[0], output);
  output.addOnTop(middle);
  this->collectMultiArguments(operatorName, current.children[2], output);
}

void CodeFormatter::sortHeaders(CodeFormatter::Element& current) {
  STACK_TRACE("CodeFormatter::sortHeaders");
  if (current.type == CodeFormatter::Element::TopLevel) {
    CodeFormatter::sortHeadersInTopLevel(current);
    return;
  }
  for (CodeFormatter::Element& child : current.children) {
    CodeFormatter::sortHeaders(child);
  }
}

void CodeFormatter::sortHeadersInTopLevel(CodeFormatter::Element& topLevel) {
  STACK_TRACE("CodeFormatter::sortHeadersInTopLevel");
  List<CodeFormatter::Element> toBeProcessed = topLevel.children;
  List<CodeFormatter::Element> includeString;
  topLevel.children.clear();
  for (CodeFormatter::Element& child : toBeProcessed) {
    if (child.type == CodeFormatter::Element::Type::IncludeLine) {
      includeString.addOnTop(child);
      continue;
    }
    this->addTopLevelElementsSorted(topLevel, includeString);
    child.indexInParent = topLevel.children.size;
    topLevel.children.addOnTop(child);
  }
  this->addTopLevelElementsSorted(topLevel, includeString);
}

void CodeFormatter::addTopLevelElementsSorted(
  CodeFormatter::Element& topLevel, List<CodeFormatter::Element>& elements
) {
  STACK_TRACE("CodeFormatter::addTopLevelElementsSorted");
  elements.quickSortAscending();
  for (CodeFormatter::Element& element : elements) {
    element.indexInParent = topLevel.children.size;
    topLevel.addChild(element);
  }
  elements.clear();
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

void CodeFormatter::applyNewLineExceptions(
  CodeFormatter::Element& left, CodeFormatter::Element& right
) {
  if (right.content == "{" && right.newLinesAfter > 0) {
    if (
      left.content == "(" &&
      left.newLinesAfter > 0 &&
      left.columnFinal + 1 < this->maximumDesiredLineLength
    ) {
      // Exception: formatting of ({ sequence.
      left.newLinesAfter = 0;
      right.whiteSpaceBefore = 0;
      return;
    }
  }
  if (left.content == "}" && right.content == ")") {
    if (
      left.previousAtom()->newLinesAfter > 0 &&
      left.newLinesAfter > 0 &&
      left.columnFinal + 1 < this->maximumDesiredLineLength
    ) {
      // We have a lonely } followed by a ) on the next line.
      left.newLinesAfter = 0;
      right.whiteSpaceBefore = 0;
      return;
    }
  }
  if (right.content != "(") {
    return;
  }
  if (left.newLinesAfter == 0) {
    return;
  }
  if (left.columnFinal + 1 >= this->maximumDesiredLineLength) {
    return;
  }
  if (left.content == ";" || left.content == "{" || left.content == "}") {
    return;
  }
  if (
    left.type == CodeFormatter::Element::Comment ||
    left.type == CodeFormatter::Element::MacroLine
  ) {
    return;
  }
  // Exception: holds naked ( on a new line is moved to the top line.
  left.newLinesAfter = 0;
}

bool CodeFormatter::mustSplitWithWhitespace(
  const CodeFormatter::Element& leftAtom,
  const CodeFormatter::Element& rightAtom
) {
  if (leftAtom.newLinesAfter > 0) {
    return false;
  }
  if (rightAtom.whiteSpaceBefore > 0) {
    // The parent nodes have already
    // requested white space before.
    return true;
  }
  if (rightAtom.content.size() == 0 || leftAtom.content.size() == 0) {
    return false;
  }
  if (leftAtom.newLinesAfter > 0) {
    return false;
  }
  if (leftAtom.content == "return" && rightAtom.content != ";") {
    return true;
  }
  char right = rightAtom.content[0];
  char left = leftAtom.content[leftAtom.content.size() - 1];
  if (left == '>' && right == '>') {
    return true;
  }
  bool startsWithSeparator = this->isSeparatorCharacter(right);
  bool previousEndsWithSeparator = this->isSeparatorCharacter(left);
  if (!previousEndsWithSeparator) {
    if (!startsWithSeparator) {
      return true;
    }
    if (rightAtom.type == CodeFormatter::Element::Quote) {
      return true;
    }
  }
  if (right == ')') {
    return false;
  }
  if (left == ')' && !startsWithSeparator) {
    return true;
  }
  if (
    rightAtom.indexInParent == 0 &&
    rightAtom.parent->type == CodeFormatter::Element::InParentheses
  ) {
    CodeFormatter::Element* previousSiblingOfParent =
    rightAtom.parent->previousSibling();
    if (previousSiblingOfParent != nullptr) {
      if (
        previousSiblingOfParent->type == CodeFormatter::Element::TypeExpression
        ||
        previousSiblingOfParent->type == CodeFormatter::Element::GreaterThan
      ) {
        return false;
      }
    }
  }
  if (
    rightAtom.content == "->" ||
    rightAtom.content == "." ||
    leftAtom.content == "->" ||
    leftAtom.content == "."
  ) {
    return false;
  }
  if (
    leftAtom.type == CodeFormatter::Element::DoubleColon ||
    rightAtom.type == CodeFormatter::Element::DoubleColon
  ) {
    return false;
  }
  if (
    leftAtom.type == CodeFormatter::Element::LessThan &&
    rightAtom.type == CodeFormatter::Element::GreaterThan
  ) {
    // Syntax such as template < >. Leave extra white space between
    // the brackets.
    return true;
  }
  if (
    leftAtom.type == CodeFormatter::Element::LessThan ||
    rightAtom.type == CodeFormatter::Element::LessThan
  ) {
    // When the type is less than/greater rather than operator, this means the
    // angle bracket is a part of a template-like syntax.
    return false;
  }
  if (rightAtom.type == CodeFormatter::Element::GreaterThan) {
    return false;
  }
  if (
    leftAtom.content == "(" ||
    leftAtom.content == "[" ||
    leftAtom.content == "{"
  ) {
    return false;
  }
  if (leftAtom.content == "!") {
    return false;
  }
  if (
    rightAtom.content == ")" ||
    rightAtom.content == "]" ||
    rightAtom.content == "}"
  ) {
    return false;
  }
  if (rightAtom.content == "[") {
    return false;
  }
  if (rightAtom.type == CodeFormatter::Element::Colon) {
    return false;
  }
  if (rightAtom.content == "(") {
    if (
      !leftAtom.isOperator() && leftAtom.type != CodeFormatter::Element::Comma
    ) {
      return false;
    }
    if (leftAtom.canBeUnaryOnTheLeft()) {
      return false;
    }
    CodeFormatter::Element* previous = leftAtom.previousAtom();
    if (previous != nullptr) {
      if (previous->type == CodeFormatter::Element::OperatorKeyWord) {
        return false;
      }
    }
  }
  if (rightAtom.content == ";") {
    return false;
  }
  if (rightAtom.isStarOrAmpersand()) {
    CodeFormatter::Element* next = rightAtom.nextAtom();
    if (next != nullptr) {
      CodeFormatter::Element* afterNext = next->nextAtom();
      if (afterNext != nullptr) {
        if (
          rightAtom.content == "*" &&
          next->isIdentifierOrAtom() &&
          afterNext->isOperator() &&
          afterNext->content != "=" &&
          afterNext->content != ","
        ) {
          return true;
        }
      }
    }
    if (
      leftAtom.type != CodeFormatter::Element::Operator &&
      leftAtom.content != ","
    ) {
      return false;
    }
  }
  if (leftAtom.isStarOrAmpersand()) {
    if (leftAtom.parent->type == CodeFormatter::Element::TypeExpression) {
      return true;
    }
    return false;
  }
  if (rightAtom.content == ",") {
    return false;
  }
  if (leftAtom.content == "~") {
    return false;
  }
  if (
    leftAtom.type == CodeFormatter::Element::OperatorKeyWord &&
    rightAtom.isOperator()
  ) {
    return false;
  }
  if (leftAtom.content == "<<" && rightAtom.content == "<") {
    return true;
  }
  if (leftAtom.content == "<" && rightAtom.content == ">") {
    return true;
  }
  return true;
}
