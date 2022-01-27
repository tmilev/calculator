// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_file_operations_encodings.h"
#include "general_logging_global_variables.h"
#include "source_code_formatter.h" 

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
  if (!this->extractCodeElements(comments)) {
    return false;
  }
  if (!this->format(comments)) {
    return false;
  }
  if (!this->writeFormatedCode(comments)) {
    return false;
  }
  if (comments != nullptr) {
    (*comments) << this->toStringLinks();
  }
  return true;
}

void CodeFormatter::addCurrentWord() {
  if (this->currentWord == "") {
    return;
  }
  this->originalElements.addOnTop(this->currentWord);
  this->currentWord = "";
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

bool CodeFormatter::processSeparatorCharacters() {
  if (this->isSeparatorCharacter(this->currentChar)) {
    this->addCurrentWord();
    this->currentWord = this->currentChar;
    this->addCurrentWord();
    return true;
  }
  this->currentWord += this->currentChar;
  return true;
}

bool CodeFormatter::processCharacterInQuotes() {
  if (!this->flagInQuotes) {
    return false;
  }
  this->currentWord += this->currentChar;
  if (this->currentChar == '"') {
    if (!this->flagPreviousIsStandaloneBackSlash) {
      this->flagInQuotes = false;
      this->addCurrentWord();
      return true;
    }
  }
  if (this->currentChar == '\\') {
    this->flagPreviousIsStandaloneBackSlash = !this->flagPreviousIsStandaloneBackSlash;
  } else {
    this->flagPreviousIsStandaloneBackSlash = false;
  }
  return true;
}

bool CodeFormatter::extractCodeElements(std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::extractCodeElements");
  (void) comments;
  this->originalElements.setExpectedSize(static_cast<signed>(this->inputCode.size()));
  this->flagInQuotes = false;
  this->flagPreviousIsStandaloneBackSlash = false;
  for (unsigned i = 0; i < this->inputCode.size(); i ++) {
    this->currentChar = this->inputCode[i];
    if (this->processCharacterInQuotes()) {
      continue;
    }
    if (this->processSeparatorCharacters()) {
      continue;
    }
  }
  this->addCurrentWord();
  return true;
}

CodeElement::CodeElement() {
  this->isIncludeLine = false;
  this->isComment = false;
  this->isInQuote = false;
  this->indentLevel = 0;
  this->currentLineLength = 0;
}

std::string CodeElement::toString() {
  std::stringstream out;
  out << "indent: " << this->indentLevel;
  return out.str();
}

CodeFormatter::CodeFormatter() {
  this->separatorCharacters = "(){} \t\n\r,:;.*&+-/[]#";
  this->doesntNeedSpaceToTheLeft = "&,:.()";
  this->doesntNeedSpaceToTheRight = "():.";
  this->pairsNotSeparatedBySpace.addOnTop(List<std::string>({"&", "&"}));
  this->pairsNotSeparatedBySpace.addOnTop(List<std::string>({"&", "*"}));
  this->pairsNotSeparatedBySpace.addOnTop(List<std::string>({"*", "&"}));
  this->maximumDesiredLineLength = 80;

  this->flagInQuotes = false;
  this->flagPreviousIsStandaloneBackSlash = false;
  this->currentChar = 0;  
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

bool CodeFormatter::format(std::stringstream* comments) {
  (void) comments;
  std::stringstream out;
  CodeElement initial;
  this->stack.addOnTop(initial);
  for (
    this->indexCurrentlyConsumed = 0;
    this->indexCurrentlyConsumed < this->originalElements.size;
    this->indexCurrentlyConsumed ++
  ) {
    this->consumeElement(out, this->originalElements[this->indexCurrentlyConsumed]);
  }
  this->transformedContent = out.str();
  return true;
}

void CodeFormatter::writeElement(std::stringstream &out, const std::string &input) {
  out << input;
  this->stack.lastObject()->currentLineLength += input.size();
  this->precedingElementThisLine = input;
}

void CodeFormatter::consumeElement(
  std::stringstream& out, const std::string& input
) {
  CodeElement& information = *this->stack.lastObject();
  if (information.isInQuote) {
    this->consumeInQuote(out, input);
    return;
  }
  if (information.isComment) {
    this->consumeInComment(out, input);
    return;
  }
  if (information.isIncludeLine) {
    this->consumeInIncludeLine(out, input);
    return;
  }
  if (input == "#") {
    std::string next = this->peekNext();
    if (next == "include") {
      CodeElement import;
      import.isIncludeLine = true;
      import.currentLineLength = information.currentLineLength;
      this->stack.addOnTop(import);
      this->writeElement(out, "#include");
      this->indexCurrentlyConsumed ++;
      return;
    }
  }
  if (this->isWhiteSpace(input)) {
    return;
  }
  if (input == "/") {
    std::string next = this->peekNext();
    if (next == "/") {
      CodeElement comment;
      comment.isComment = true;
      comment.currentLineLength = information.currentLineLength;
      this->stack.addOnTop(comment);
      this->writeElement(out, "//");
      this->indexCurrentlyConsumed ++;
      return;
    }
  }
  if (input == "}") {
    this->popStack();
    out << "}\n";
    return;
  }
  std::string incoming;
  if (this->shouldSeparateWithSpace(this->precedingElementThisLine, input)) {
    incoming = " " + input;
  } else {
    incoming = input;
  }

  if (information.currentLineLength + static_cast<signed>(incoming.size()) > this->maximumDesiredLineLength) {
    this->breakLine(out);
    this->indent(out);
    incoming = input;
  }
  this->writeElement(out, incoming);
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

std::string CodeFormatter::peekNext() const {
  int nextIndex = this->indexCurrentlyConsumed + 1;
  if (nextIndex >= this->originalElements.size) {
    return "";
  }
  return this->originalElements[nextIndex];
}

void CodeFormatter::consumeInIncludeLine(std::stringstream &out, const std::string &input) {
  if (input == "\n") {
    this->popStack();
    this->breakLine(out);
    std::string next = this->peekNext();
    if (next != "#") {
      out << "\n";
    }
    return;
  }
  this->writeElement(out, input);
  return;
}

void CodeFormatter::consumeInQuote(std::stringstream &out, const std::string &input) {
  CodeElement& last = *this->stack.lastObject();
  if (input == "\\") {
    out << input;
    out << this->peekNext();
    this->indexCurrentlyConsumed ++;
    last.currentLineLength += 2;
    return;
  }
  if (input == "\"") {
    out << input;
    last.currentLineLength += input.size();
    this->popStack();
    return;
  }
  out << input;
  last.currentLineLength += input.size();
}

void CodeFormatter::consumeInComment(
  std::stringstream& out, const std::string& input
) {
  if (input == "\n") {
    this->popStack();
    this->breakLine(out);
    return;
  }
  CodeElement& last = *this->stack.lastObject();
  if (
    static_cast<int>(input.size()) + last.currentLineLength >
    this->maximumDesiredLineLength
  ) {
    this->breakLine(out);
    this->indent(out);
    out << "//";
    last.currentLineLength += 2;
  }
  out << input;
  last.currentLineLength += input.size();
  return;
}

void CodeFormatter::popStack() {
  CodeElement removed = this->stack.popLastObject();
  if (this->stack.size == 0) {
    CodeElement empty;
    this->stack.addOnTop(empty);
  }
  this->stack.lastObject()->currentLineLength = removed.currentLineLength;
}

void CodeFormatter::indent(std::stringstream &out) {
  CodeElement& information = *this->stack.lastObject();
  if (information.currentLineLength != 0) {
    return;
  }
  int spaceCount = information.indentLevel * tabLength;
  information.currentLineLength = spaceCount;
  for (int i = 0; i < spaceCount; i ++) {
    out << " ";
  }
}

void CodeFormatter::breakLine(std::stringstream& out) {
  out << "\n";
  this->stack.lastObject()->currentLineLength = 0;
  this->precedingElementThisLine = "";
}

bool CodeFormatter::isWhiteSpace(const std::string& input) {
  return input == "\n" || input == "\t" || input == " ";
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
