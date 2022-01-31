// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_file_operations_encodings.h"
#include "general_logging_global_variables.h"
#include "source_code_formatter.h" 
#include "general_strings.h"

CodeFormatter::Element::Element() {
  this->indentLevel = 0;
  this->currentLineLength = 0;
  this->type = CodeFormatter::Element::Unknown;
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
  out <<  "[" << this->toStringType(this->type) << "]";
  return out.str();
}

std::string CodeFormatter::Element::toStringType(CodeFormatter::Element::Type input) {
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
    return "()";
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
  case CodeFormatter::Element::FunctionDeclaration:
    return "functionDeclaration";
  case CodeFormatter::Element::FunctionWithArguments:
    return "functionWithArguments";
  case CodeFormatter::Element::TypeAndIdentifier:
    return "typeAndIdentifier";
  case CodeFormatter::Element::TypeExpression:
    return "typeExpression";
  case CodeFormatter::Element::CaseKeyWord:
    return "case keyword";
  case CodeFormatter::Element::CaseClause:
    return "case clause";
  case CodeFormatter::Element::CaseClauseStart:
    return "case clause start";
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
  case CodeFormatter::Element::DefaultKeyWord:
    return "default";
  case CodeFormatter::Element::ConstructorExternal:
    return "default";
  }
  return "????";
}

void CodeFormatter::Element::appendExpression(const CodeFormatter::Element &other) {
  this->appendType(other, CodeFormatter::Element::Expression);
}

void CodeFormatter::Element::appendIdentifier(const CodeFormatter::Element &other) {
  this->appendType(other, CodeFormatter::Element::Identifier);
}

void CodeFormatter::Element::appendType(const CodeFormatter::Element &other, CodeFormatter::Element::Type inputType) {
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

void CodeFormatter::Element::makeFrom2(CodeFormatter::Element::Type inputType, const Element &left, const Element &right) {
  if (this == & left || this == & right) {
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

void CodeFormatter::Element::makeFrom1(CodeFormatter::Element::Type inputType, const Element &child) {
  if (this == & child) {
    CodeFormatter::Element childCopy = child;
    this->makeFrom1(inputType, childCopy);
    return;
  }
  this->clear();
  this->type = inputType;
  this->children.addOnTop(child);
}

void CodeFormatter::Element::makeFrom3(CodeFormatter::Element::Type inputType, const Element &left, const Element &middle, const Element &right) {
  if (this == & left || this == & right || this == &middle) {
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

void CodeFormatter::Element::appendCommand(const CodeFormatter::Element &other) {
  this->appendType(other, CodeFormatter::Element::CommandList);
}

bool CodeFormatter::Element::isSuitableForTypeExpression() const {
  return
  this->isIdentifierOrAtom() ||
  this->type == CodeFormatter::Element::TypeExpression ||
  this->type == CodeFormatter::Element::TypeKeyWord;
}

bool CodeFormatter::Element::isSuitableForParenthesesEnclosure() const {
  return
  this->isCommandListOrCommandOrClause() ||
  this->isExpressionIdentifierOrAtom() ||
  this->type == CodeFormatter::Element::TypeAndIdentifier ||
  this->type == CodeFormatter::Element::TypeKeyWord ||
  this->type == CodeFormatter::Element::TypeExpression ||
  this->type == CodeFormatter::Element::CommaList
  ;
}

bool CodeFormatter::Element::isSuitableForCommaListElement() const {
  return
  this->isExpressionIdentifierOrAtom() ||
  this->type == CodeFormatter::Element::TypeAndIdentifier
  ;
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

bool CodeFormatter::Element::isCommandOrClause() const {
  return
  this->type == CodeFormatter::Element::Command ||
  this->type == CodeFormatter::Element::CaseClause
  ;
}

bool CodeFormatter::Element::isCommandListOrCommandOrClause() const {
  return
  this->type == CodeFormatter::Element::Command ||
  this->type == CodeFormatter::Element::CommandList ||
  this->type == CodeFormatter::Element::CaseClause
  ;
}

bool CodeFormatter::Element::isSuitableForTopLevel() const {
  return
  this->type == CodeFormatter::Element::IncludeLine ||
  this->type == CodeFormatter::Element::Command ||
  this->type == CodeFormatter::Element::Comment ||
  this->type == CodeFormatter::Element::CommentMultiline
  ;
}

bool CodeFormatter::Element::isSuitableForCommand() const {
  return
  this->isExpressionIdentifierOrAtom() ||
  this->type == CodeFormatter::Element::FunctionWithArguments ||
  this->type == CodeFormatter::Element::TypeAndIdentifier
  ;
}

bool CodeFormatter::Element::isParenthesesBlock() const {
  return
  this->type == CodeFormatter::Element::InParentheses;
}

bool CodeFormatter::Element::isCodeBlock() const {
  return
  this->type == CodeFormatter::Element::CodeBlock;
}

bool CodeFormatter::Element::isTypeWord() const {
  return
  this->type == CodeFormatter::Element::TypeKeyWord;
}

bool CodeFormatter::Element::isOperator() const {
  return this->isStarOrAmpersand() || this->type == CodeFormatter::Element::Operator;
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
  this->type == CodeFormatter::Element::Atom
  ;
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
    this->flagPreviousIsStandaloneBackSlash = !this->flagPreviousIsStandaloneBackSlash;
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
    this->flagPreviousIsStandaloneBackSlash = !this->flagPreviousIsStandaloneBackSlash;
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
  if (this->currentChar == '/' && this->currentWord.content[this->currentWord.content.size() - 1] == '*') {
    this->addCurrentWord();
  }
  return true;
}

bool CodeFormatter::Words::extractCodeElements(std::stringstream* comments) {
  MacroRegisterFunctionWithName("CodeFormatter::Words::extractCodeElements");
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
  if (!FileOperations::loadFileToStringVirtual(this->inputFileName, this->inputCode, false, comments)) {
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

std::string CodeFormatter::toStringLinks() {
  MacroRegisterFunctionWithName("SourceCodeFormatter::toStringLinks");
  std::stringstream out;
  out
  << "<a href = '"
  << this->inputFileName
  << "'>"
  << this->inputFileName << "</a>, ";
  out
  << "<a href = '"
  << this->outputFileName
  << "'>"
  << this->outputFileName << "</a>. ";
  return out.str();
}

bool CodeFormatter::formatCPPDirectory(const std::string& inputDirectory, std::stringstream* comments) {
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
  List<std::string> allFiles, newFileNames, oldFileNames, allFileExtensions;
  if (!FileOperations::getFolderFileNamesVirtual(directory, allFiles, &allFileExtensions, false, false, comments)) {
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
    if (!formatter.formatCPPSourceCode(oldFileNames[i], newFileNames[i], comments)) {
      return false;
    }
  }
  return true;
}

bool CodeFormatter::formatCPPSourceCode(
  const std::string& inputFileName,
  const std::string& inputOutputFileNameEmptyForAuto,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::formatCPPSourceCode");
  if (!this->initializeFileNames(inputFileName, inputOutputFileNameEmptyForAuto, comments)) {
    return false;
  }
  this->words.initialize(*this);
  if (!this->words.extractCodeElements(comments)) {
    return false;
  }
  this->processor.initialize(*this);
  this->processor.consumeElements();
  std::stringstream out;
  this->formatTopLevel(out, this->processor.code);
  this->transformedContent = out.str();
  if (!this->writeFormatedCode(comments)) {
    return false;
  }
  if (comments != nullptr) {
    (*comments) << this->toStringLinks();
  }
  return true;
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

CodeFormatter::CodeFormatter() {
  this->separatorCharacters = "(){} \t\n\r,:;.*&+-/[]#\"'=><";
  this->doesntNeedSpaceToTheLeft = "&,:.()";
  this->doesntNeedSpaceToTheRight = "():.";
  this->pairsNotSeparatedBySpace.addOnTop(List<std::string>({"&", "&"}));
  this->pairsNotSeparatedBySpace.addOnTop(List<std::string>({"&", "*"}));
  this->pairsNotSeparatedBySpace.addOnTop(List<std::string>({"*", "&"}));
  this->elementTypes.setKeyValue(":", CodeFormatter::Element::Colon);
  this->elementTypes.setKeyValue(";", CodeFormatter::Element::SemiColon);
  this->elementTypes.setKeyValue("(", CodeFormatter::Element::LeftParenthesis);
  this->elementTypes.setKeyValue(")", CodeFormatter::Element::RightParenthesis);
  this->elementTypes.setKeyValue("{", CodeFormatter::Element::LeftCurlyBrace);
  this->elementTypes.setKeyValue("}", CodeFormatter::Element::RightCurlyBrace);
  this->elementTypes.setKeyValue("[", CodeFormatter::Element::LeftBracket);
  this->elementTypes.setKeyValue("]", CodeFormatter::Element::RightBracket);
  this->elementTypes.setKeyValue("&", CodeFormatter::Element::Ampersand);
  this->elementTypes.setKeyValue("*", CodeFormatter::Element::Star);
  this->elementTypes.setKeyValue(",", CodeFormatter::Element::Comma);
  this->elementTypes.setKeyValue("const", CodeFormatter::Element::ConstKeyWord);
  this->elementTypes.setKeyValue("case", CodeFormatter::Element::CaseKeyWord);
  this->elementTypes.setKeyValue("default", CodeFormatter::Element::DefaultKeyWord);
  this->typeKeyWords.addListOnTop(List<std::string>({
    "bool",
    "void",
  }));
  this->controlKeyWords.addListOnTop(List<std::string>({
    "for",
    "if",
    "while",
  }));
  this->operatorList.addListOnTop(List<std::string>({
    "+",
    "-",
    ">",
    "<",
    "=",
    "/",
    "*",
    "|",
    ".",
    "->",
    "==",
    ">>",
    "<<",
    "&&",
    "||"
  }));
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
  for (unsigned i = 0; i < this->doesntNeedSpaceToTheRight.size(); i ++) {
    std::string incoming = this->doesntNeedSpaceToTheRight.substr(i, 1);
    this->doesntNeedSpaceToTheRightContainer.addOnTop(incoming);
  }
}

void CodeFormatter::formatTopLevel(std::stringstream &out, CodeFormatter::Element &input) {
  MacroRegisterFunctionWithName("CodeFormatter::formatTopLevel");
  for (int i = 0; i < input.children.size; i ++) {
    CodeFormatter::Element& current = input.children[i];
    if (current.type == CodeFormatter::Element::Dummy) {
      continue;
    }
    if (i > 1) {
      CodeFormatter::Element& previous = input.children[i - 1];
      if (previous.type == CodeFormatter::Element::IncludeLine && current.type != CodeFormatter::Element::IncludeLine) {
        out << "\n";
      }
    }
    this->format(out, current);
    out << "\n";
  }
}

void CodeFormatter::format(std::stringstream &out, Element &input) {
  MacroRegisterFunctionWithName("CodeFormatter::format");
  if (input.type == CodeFormatter::Element::TopLevel) {
    this->formatTopLevel(out, input);
    return;
  }
  if (input.content != "") {
    out << input.content;
    return;
  }
  for (int i = 0; i < input.children.size; i ++) {
    this->format(out, input.children[i]);
  }
}

CodeFormatter::Processor::Processor() {
  this->owner = nullptr;
  global.comments << "DEBUG: preparing report!";
  this->flagPrepareReport = true;
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
  global.comments << "DEBUG: " << this->debugLog;
  this->code.type = CodeFormatter::Element::FileContent;
  this->code.content = "";
  this->code.children = this->stack;
}

void CodeFormatter::Processor::consumeOneElement(CodeFormatter::Element& incoming) {
  MacroRegisterFunctionWithName("CodeFormatter::Processor::consumeOneElement");
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
      global.fatal << "Too many rule applications without reducing the stack. " << global.fatal;
    }
  }
}

bool CodeFormatter::Processor::applyOneRule() {
  CodeFormatter::Element& last = this->stack[this->stack.size - 1];
  CodeFormatter::Element& secondToLast = this->stack[this->stack.size - 2];
  CodeFormatter::Element& thirdToLast = this->stack[this->stack.size - 3];
  CodeFormatter::Element& fourthToLast = this->stack[this->stack.size - 4];
  if (secondToLast.content == "#" && last.content == "include") {
    secondToLast.content = "#include";
    secondToLast.type = CodeFormatter::Element::IncludeLineStart;
    return this->removeLast();
  }
  if (secondToLast.type == CodeFormatter::Element::IncludeLineStart && last.type == CodeFormatter::Element::EndLine) {
    secondToLast.type = CodeFormatter::Element::IncludeLine;
    return this->removeLast();
  }
  if (secondToLast.type == CodeFormatter::Element::IncludeLineStart && last.type != CodeFormatter::Element::EndLine) {
    secondToLast.content.append(last.content);
    return this->removeLast();
  }
  if (this->owner->isWhiteSpace(last.content)) {
    this->lastRuleName = "trim whitepace";
    return this->removeLast();
  }
  if (last.type == CodeFormatter::Element::Unknown && this->owner->isTypeKeyWord(last.content)) {
    this->lastRuleName = "type key word";
    last.type = CodeFormatter::Element::TypeKeyWord;
    return true;
  }
  if (last.type == CodeFormatter::Element::Unknown && this->owner->isControlKeyWord(last.content)) {
    this->lastRuleName = "control key word";
    last.type = CodeFormatter::Element::ControlKeyWord;
    return true;
  }
  if (
    thirdToLast.type == CodeFormatter::Element::CaseClauseStart &&
    secondToLast.isCommandListOrCommandOrClause() && (
      last.type == CodeFormatter::Element::CaseClauseStart ||
      last.type == CodeFormatter::Element::RightCurlyBrace
  )) {
    thirdToLast.makeFrom2(CodeFormatter::Element::CaseClause, thirdToLast, secondToLast);
    return this->removeBelowLast(1);
  }
  if (last.type == CodeFormatter::Element::Quote) {
    this->lastRuleName = "quote to expression";
    last.type = CodeFormatter::Element::Expression;
    return true;
  }
  if (secondToLast.type == CodeFormatter::Element::TopLevel && last.isSuitableForTopLevel()) {
    secondToLast.children.addOnTop(last);
    return this->removeLast();
  }
  if (secondToLast.type == CodeFormatter::Element::ControlKeyWord && last.type == CodeFormatter::Element::InParentheses) {
    this->lastRuleName = "control wants code block";
    secondToLast.makeFrom2(CodeFormatter::Element::ControlWantsCodeBlock, secondToLast, last);
    return this->removeLast();
  }
  if (secondToLast.type == CodeFormatter::Element::ControlWantsCodeBlock && last.isCodeBlock()) {
    this->lastRuleName = "control and code block";
    secondToLast.makeFrom2(CodeFormatter::Element::Command, secondToLast, last);
    return this->removeLast();
  }
  if (last.type == CodeFormatter::Element::Unknown && this->owner->elementTypes.contains(last.content)) {
    this->lastRuleName = "look up element type";
    last.type = this->owner->elementTypes.getValueNoFail(last.content);
    return true;
  }
  if (secondToLast.type == CodeFormatter::Element::Colon && last.type == CodeFormatter::Element::Colon) {
    this->lastRuleName = "double-colon";
    secondToLast.type = CodeFormatter::Element::DoubleColon;
    secondToLast.content = "::";
    return this->removeLast();
  }
  if (secondToLast.type == CodeFormatter::Element::FunctionDeclaration && last.type == CodeFormatter::Element::ConstKeyWord) {
    secondToLast.children.addOnTop(last);
    return this->removeLast();
  }
  if (last.type == CodeFormatter::Element::Unknown && StringRoutines::isLatinLetterOrDigitSequence(last.content)) {
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

  if (fourthToLast.type == CodeFormatter::Element::CaseKeyWord && secondToLast.type == CodeFormatter::Element::Colon) {
    this->lastRuleName = "case clause start";
    fourthToLast.makeFrom3(CodeFormatter::Element::CaseClauseStart, fourthToLast, thirdToLast, secondToLast);
    return this->removeBelowLast(2);
  }
  if (thirdToLast.type == CodeFormatter::Element::DefaultKeyWord && secondToLast.type == CodeFormatter::Element::Colon) {
    this->lastRuleName = "default clause";
    thirdToLast.makeFrom2(CodeFormatter::Element::CaseClauseStart, thirdToLast, secondToLast);
    return this->removeBelowLast(1);
  }
  if (secondToLast.type == CodeFormatter::Element::LeftParenthesis && last.type == CodeFormatter::Element::RightParenthesis) {
    this->lastRuleName = "parentheses match";
    secondToLast.makeFrom2(CodeFormatter::Element::InParentheses, secondToLast, last);
    return this->removeLast();
  }
  if (thirdToLast.type == CodeFormatter::Element::LeftParenthesis &&
    secondToLast.isSuitableForParenthesesEnclosure() &&
    last.type == CodeFormatter::Element::RightParenthesis
  ) {
    this->lastRuleName = "(command list)";
    CodeFormatter::Element incoming;
    incoming.children.addOnTop(thirdToLast);
    incoming.children.addOnTop(secondToLast);
    incoming.children.addOnTop(last);
    incoming.type = CodeFormatter::Element::InParentheses;
    thirdToLast = incoming;
    return this->removeLast(2);
  }
  if (thirdToLast.type == CodeFormatter::Element::LeftBracket &&
    secondToLast.isExpressionIdentifierOrAtom()
    && last.type == CodeFormatter::Element::RightBracket
  ) {
    this->lastRuleName = "[expression]";
    thirdToLast.makeFrom3(CodeFormatter::Element::Expression, thirdToLast, secondToLast, last);
    return this->removeLast(2);
  }
  if (secondToLast.type == CodeFormatter::Element::LeftCurlyBrace && last.type == CodeFormatter::Element::RightCurlyBrace) {
    this->lastRuleName = "{}";
    secondToLast.makeFrom2(CodeFormatter::Element::CodeBlock, secondToLast, last);
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
    secondToLast.type == CodeFormatter::Element::Operator && last.type == CodeFormatter::Element::Operator &&
    this->owner->isOperator(secondToLast.content + last.content)
  ) {
    this->lastRuleName = "add operators";
    secondToLast.content = secondToLast.content + last.content;
    return this->removeLast();
  }
  if (secondToLast.type == CodeFormatter::Element::FunctionWithArguments && last.type == CodeFormatter::Element::Operator) {
    this->lastRuleName = "function with arguments to expression";
    secondToLast.makeFrom1(CodeFormatter::Element::Expression, secondToLast);
    return true;
  }
  if (secondToLast.isExpressionOrAtom() && last.isOperator()) {
    this->lastRuleName = "expression operator";
    secondToLast.appendExpression(last);
    return this->removeLast();
  }
  if (
    thirdToLast.type == CodeFormatter::Element::ConstKeyWord &&
    secondToLast.isExpressionIdentifierOrAtom() &&
    last.type != CodeFormatter::Element::Colon &&
    last.type != CodeFormatter::Element::DoubleColon
  ) {
    this->lastRuleName = "const something";
    thirdToLast.makeFrom2(CodeFormatter::Element::TypeExpression, thirdToLast, secondToLast);
    return this->removeBelowLast(1);
  }
  if (
    secondToLast.type == CodeFormatter::Element::TypeExpression &&
    last.isStarOrAmpersand()
  ) {
    secondToLast.children.addOnTop(last);
    return this->removeLast();
  }
  if (
    secondToLast.type == CodeFormatter::Element::Ampersand &&
    last.type == CodeFormatter::Element::Ampersand
  ) {
    secondToLast.clear();
    secondToLast.content = "&&";
    secondToLast.type = CodeFormatter::Element::Operator;
    return this->removeLast();
  }
  if (
    thirdToLast.isSuitableForTypeExpression() &&
    secondToLast.isIdentifierOrAtom() &&
    last.type != CodeFormatter::Element::Colon &&
    last.type != CodeFormatter::Element::DoubleColon
  ) {
    this->lastRuleName = "type and identifier";
    thirdToLast.makeFrom2(CodeFormatter::Element::TypeAndIdentifier, thirdToLast, secondToLast);
    return this->removeBelowLast(1);
  }

  if (thirdToLast.isExpressionIdentifierOrAtom() && secondToLast.isExpressionIdentifierOrAtom() && last.type != CodeFormatter::Element::Colon && last.type != CodeFormatter::Element::DoubleColon) {
    this->lastRuleName = "expression and atom";
    thirdToLast.appendExpression(secondToLast);
    return this->removeBelowLast(1);
  }
  if (secondToLast.isSuitableForCommand() && last.type == CodeFormatter::Element::SemiColon) {
    this->lastRuleName = "expression and semicolon";
    secondToLast.appendExpression(last);
    secondToLast.type = CodeFormatter::Element::Command;
    return this->removeLast();
  }
  if (
    thirdToLast.type == CodeFormatter::Element::CommaList &&
    secondToLast.isSuitableForCommaListElement() &&
    last.type == CodeFormatter::Element::Comma
  ) {
    thirdToLast.children.addOnTop(secondToLast);
    thirdToLast.children.addOnTop(last);
    return this->removeLast(2);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::CommaList &&
    secondToLast.isSuitableForCommaListElement() && (
      last.type == CodeFormatter::Element::RightParenthesis ||
      last.type == CodeFormatter::Element::RightCurlyBrace ||
      last.type == CodeFormatter::Element::RightBracket
  )) {
    thirdToLast.children.addOnTop(secondToLast);
    return this->removeBelowLast(1);
  }
  if (secondToLast.isSuitableForCommaListElement() && last.type == CodeFormatter::Element::Comma) {
    this->lastRuleName = "expression, ";
    secondToLast.makeFrom2(CodeFormatter::Element::CommaList, secondToLast, last);
    return this->removeLast();
  }
  if (thirdToLast.isCommandListOrCommandOrClause() && secondToLast.isCommandOrClause()) {
    this->lastRuleName = "commandList command something";
    thirdToLast.appendCommand(secondToLast);
    return this->removeBelowLast(1);
  }
  if (secondToLast.isCommandListOrCommandOrClause() && last.isCommandOrClause()) {
    this->lastRuleName = "commandList command";
    secondToLast.appendCommand(last);
    return this->removeLast();
  }
  if (thirdToLast.isCommandListOrCommandOrClause() && secondToLast.isExpressionIdentifierOrAtom() && last.type == CodeFormatter::Element::RightParenthesis) {
    this->lastRuleName = "commandList expression)";
    thirdToLast.appendCommand(secondToLast);
    return this->removeBelowLast(1);
  }
  if (
    thirdToLast.type == CodeFormatter::Element::LeftCurlyBrace &&
    secondToLast.isCommandListOrCommandOrClause() &&
    last.type == CodeFormatter::Element::RightCurlyBrace
  ) {
    CodeFormatter::Element codeBlock;
    codeBlock.type = CodeFormatter::Element::CodeBlock;
    codeBlock.children.addOnTop(thirdToLast);
    codeBlock.children.addOnTop(secondToLast);
    codeBlock.children.addOnTop(last);
    thirdToLast = codeBlock;
    return this->removeLast(2);
  }
  if (secondToLast.isTypeOrIdentifierOrExpression() && last.isParenthesesBlock()) {
    this->lastRuleName = "identifier(...)";
    secondToLast.makeFrom2(CodeFormatter::Element::FunctionWithArguments, secondToLast, last);
    return this->removeLast();
  }
  if (secondToLast.type == CodeFormatter::Element::TypeAndIdentifier && last.isParenthesesBlock()) {
    this->lastRuleName = "type_identifier + parentheses block";
    secondToLast.makeFrom2(CodeFormatter::Element::FunctionDeclaration, secondToLast, last);
    return this->removeLast();
  }
  if ( (secondToLast.type == CodeFormatter::Element::FunctionDeclaration || secondToLast.type == CodeFormatter::Element::FunctionWithArguments) && last.isCodeBlock()) {
    this->lastRuleName = "function declaration + code block";
    secondToLast.makeFrom2(CodeFormatter::Element::Command, secondToLast, last);
    return this->removeLast();
  }
  if (thirdToLast.isIdentifierOrAtom() && secondToLast.isParenthesesBlock() && last.isCodeBlock()) {
    this->lastRuleName = "atom(...){...}";
    thirdToLast.makeFrom3(CodeFormatter::Element::ConstructorExternal, thirdToLast, secondToLast, last);
    return this->removeLast(2);
  }
  return false;
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
  this->debugLog += this->toStringStack() + "[[" + this->lastRuleName + "]]<br>";
}

std::string CodeFormatter::Processor::toStringStack() {
  std::stringstream out;
  for (int i = this->owner->dummyElements; i < this->stack.size; i ++) {
    out << this->stack[i].toString() << " ";
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

bool CodeFormatter::isTypeKeyWord(const std::string &input) {
  return this->typeKeyWords.contains(input);
}

bool CodeFormatter::isControlKeyWord(const std::string &input) {
  return this->controlKeyWords.contains(input);
}

bool CodeFormatter::isWhiteSpace(const std::string& input) {
  return input == "\n" || input == "\t" || input == " " || input == "\r";
}

bool CodeFormatter::writeFormatedCode(std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::writeFormatedCode");
  std::fstream fileOut;
  if (!FileOperations::openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(
    fileOut, this->outputFileName, false, true, false
  )) {
    if (comments != nullptr) {
      *comments << "Failed to open source code formatting output file. ";
    }
    return false;
  }
  fileOut << this->transformedContent;
  return true;
}
