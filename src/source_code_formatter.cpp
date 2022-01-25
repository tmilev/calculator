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
  CodeElement incoming;
  incoming.content = this->currentWord;
  this->originalElements.addOnTop(incoming);
  this->currentWord = "";
}

bool CodeFormatter::isSeparatorCharacter(char input) {
  unsigned char inputUnsigned = static_cast<unsigned char>(input);
  return this->separatorCharactersMap[inputUnsigned];
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
  this->indentLevel = 0;
}

std::string CodeElement::toString() {
  std::stringstream out;
  out << "[";
  out << this->content;
  out << "]";
  return out.str();
}

CodeFormatter::CodeFormatter() {
  this->flagInQuotes = false;
  this->flagPreviousIsStandaloneBackSlash = false;
  this->currentChar = 0;
  this->separatorCharacters = "(){} \t\n\r,:;.*&+-/[]";
  this->separatorCharactersMap.initializeFillInObject(256, false);
  this->currentLineLength = 0;
  this->currentLineIndent = 0;
  this->previousLineIndent = 0;
  this->previousLineLength = 0;
  this->maximumDesiredLineLength = 120;
  for (unsigned i = 0; i < this->separatorCharacters.size(); i ++) {
    this->separatorCharactersMap[static_cast<signed>(this->separatorCharacters[i])] = true;
  }
}

bool CodeFormatter::format(std::stringstream* comments) {
  (void) comments;
  std::stringstream out;
  for (int i = 0; i < this->originalElements.size; i ++) {
    out << this->originalElements[i].content;
  }
  this->transformedContent = out.str();
  return true;
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
