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

std::string CodeFormatter::ToStringLinks() {
  MacroRegisterFunctionWithName("SourceCodeFormatter::ToStringLinks");
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

bool CodeFormatter::FormatCPPDirectory(const std::string& inputDirectory, std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::FormatCPPDirectory");
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
    CodeFormatter theFormatter;
    if (!theFormatter.FormatCPPSourceCode(oldFileNames[i], newFileNames[i], comments)) {
      return false;
    }
  }
  return true;
}

bool CodeFormatter::FormatCPPSourceCode(
  const std::string& inputFileName,
  const std::string& inputOutputFileNameEmptyForAuto,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::FormatCPPSourceCode");
  if (!this->initializeFileNames(inputFileName, inputOutputFileNameEmptyForAuto, comments)) {
    return false;
  }
  if (!this->ExtractCodeElements(comments)) {
    return false;
  }
  if (!this->ApplyFormattingRules(comments)) {
    return false;
  }
  if (!this->WriteFormatedCode(comments)) {
    return false;
  }
  if (comments != nullptr) {
    (*comments) << this->ToStringLinks();
  }
  return true;
}

void CodeFormatter::AddCurrentWord() {
  this->AddWordToOriginals(this->currentWord);
  this->currentWord = "";
}

bool CodeFormatter::AddWordToTarget(const std::string& incomingString, List<CodeElement>& output) {
  if (incomingString == "") {
    return true;
  }
  CodeElement incoming;
  this->SetContentComputeType(incomingString, incoming);
  output.addOnTop(incoming);
  return true;
}

bool CodeFormatter::AddWordToTransformed(const std::string& incomingString) {
  return this->AddWordToTarget(incomingString, this->transformedElements);
}

bool CodeFormatter::AddWordToOriginals(const std::string& incomingString) {
  return this->AddWordToTarget(incomingString, this->originalElements);
}

bool CodeFormatter::isSeparatorCharacter(char input) {
  unsigned char inputUnsigned = static_cast<unsigned char>(input);
  return this->separatorCharactersMap[inputUnsigned];
}

bool CodeFormatter::processSeparatorCharacters() {
  if (this->isSeparatorCharacter(this->currentChar)) {
    this->AddCurrentWord();
    this->currentWord = this->currentChar;
    this->AddCurrentWord();
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
      this->AddCurrentWord();
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

void CodeFormatter::SetContentComputeType(const std::string& input, CodeElement& output) {
  output.content = input;
  if (this->builtInTypes.contains(input)) {
    output.type = this->builtInTypes.getValueNoFail(input);
  } else {
    output.type = "";
  }
}

bool CodeFormatter::ExtractCodeElements(std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::ExtractCodeElements");
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
  this->AddCurrentWord();
  return true;
}

bool CodeFormatter::DecreaseStack(int numberToPop) {
  this->transformedElements.setSize(this->transformedElements.size - numberToPop);
  return true;
}

std::string CodeElement::toString() {
  std::stringstream out;
  out << "[";
  if (this->content == "\n") {
    out << "\\n";
  } else if (this->type == "whiteSpace" || this->content == " " || this->content == "  ") {
    out << "[whiteSpace] " << this->content.size();
  } else {
    out << this->content;
  }
  out << "]";
  return out.str();
}

std::string CodeFormatter::ToStringTransformed6() {
  std::stringstream out;
  int lastIndex = this->transformedElements.size - 1;
  CodeElement& last = this->transformedElements[lastIndex];
  CodeElement& secondToLast = this->transformedElements[lastIndex - 1];
  CodeElement& thirdToLast = this->transformedElements[lastIndex - 2];
  CodeElement& fourthToLast = this->transformedElements[lastIndex - 3];
  CodeElement& fifthToLast = this->transformedElements[lastIndex - 4];
  CodeElement& sixthToLast = this->transformedElements[lastIndex - 5];
  out
  << "Indent: " << this->currentLineIndent << ", length: " << this->currentLineLength
  << ", previous indent: " <<  this->previousLineIndent << ", previous length: "
  << this->previousLineLength;
  out << "<br>Last 6: "
  << sixthToLast .toString()
  << fifthToLast .toString()
  << fourthToLast.toString()
  << thirdToLast .toString()
  << secondToLast.toString()
  << last        .toString();
  return out.str();
}

bool CodeFormatter::ApplyOneRule(std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::ApplyOneRule");
  (void) comments;
  int lastIndex = this->transformedElements.size - 1;
  CodeElement& last = this->transformedElements[lastIndex];
  CodeElement& secondToLast = this->transformedElements[lastIndex - 1];
  CodeElement& thirdToLast = this->transformedElements[lastIndex - 2];
  CodeElement& fourthToLast = this->transformedElements[lastIndex - 3];
  CodeElement& fifthToLast = this->transformedElements[lastIndex - 4];
  CodeElement& sixthToLast = this->transformedElements[lastIndex - 5];
  if (
    (last.type == "whiteSpace" || last.content == " ") &&
    (secondToLast.type == "whiteSpace" || secondToLast.content == " ")
  ) {
    secondToLast.content = secondToLast.content + last.content;
    secondToLast.type = "whiteSpace";
    return this->DecreaseStack(1);
  }
  if (secondToLast.content == "\\" && last.content == "\\") {
    secondToLast.content = "\\\\";
    secondToLast.type = "commentOneLine";
    return this->DecreaseStack(1);
  }
  if (secondToLast.type == "commentOneLine" && last.content != "\n") {
    secondToLast.content += last.content;
    return this->DecreaseStack(1);
  }
  if (last.content == "\n" && secondToLast.content == ")" && this->currentLineLength > this->maximumDesiredLineLength) {
    //if (comments != 0) {
    //  *comments << "<hr>Rule 0.<br>" << this->ToStringTransformed6();
    //}
    this->SetContentComputeType("\n", secondToLast);
    this->previousLineIndent = this->currentLineIndent;
    last.type = "whiteSpace";
    last.content.resize(static_cast<unsigned>(this->currentLineIndent));
    for (unsigned i = 0; i < static_cast<unsigned>(this->currentLineIndent); i ++) {
      last.content[i] = ' ';
    }
    return this->AddWordToTransformed(")");
  }
  if (
    this->previousLineLength > this->maximumDesiredLineLength &&
    fifthToLast.content != "\n" &&
    fourthToLast.content == ")" &&
    thirdToLast.content == " " &&
    secondToLast.content == "{" &&
    last.content == "\n"
  ) {
    //if (comments != 0) {
    //  *comments << "<hr>Rule 1.<br>" << this->ToStringTransformed6();
    //}
    this->SetContentComputeType("\n", fourthToLast);
    this->previousLineIndent = this->currentLineIndent;
    thirdToLast.type = "whiteSpace";
    thirdToLast.content.resize(static_cast<unsigned>(this->currentLineIndent));
    for (unsigned i = 0; i < static_cast<unsigned>(this->currentLineIndent); i ++) {
      thirdToLast.content[i] = ' ';
    }
    this->SetContentComputeType(")", secondToLast);
    this->SetContentComputeType(" ", last);
    this->AddWordToTransformed("{");
    this->previousLineLength = static_cast<int>(thirdToLast.content.size() + 3);
    this->currentLineLength = 0;
    this->currentLineIndent = 0;
    return this->AddWordToTransformed("\n");
  }
  if (
    thirdToLast.content.size() > 1 &&
    secondToLast.content == "\n" &&
    last.content == "("
  ) {
    //if (comments != 0) {
    //  *comments << "<hr>Rule 3.<br>" << this->ToStringTransformed6();
    //}
    this->SetContentComputeType("(", secondToLast);
    this->SetContentComputeType("\n", last);
    this->previousLineLength = this->currentLineLength;
    this->currentLineLength = 2;
    this->currentLineIndent = 0;
    this->AddWordToTransformed(" ");
    return this->AddWordToTransformed(" ");
  }
  if (
    (fourthToLast.content == ")" || fourthToLast.content == "else") &&
    thirdToLast.type == "\n" &&
    secondToLast.content == "whiteSpace" &&
    last.content == "{"
  ) {
    thirdToLast.content = " {";
    thirdToLast.type = "openCurlyBrace";
    this->SetContentComputeType("\n", secondToLast);
    last.content += " ";
    return true;
  }
  if (
    (thirdToLast.content == ")" || thirdToLast.content == "else") &&
    secondToLast.content == "\n" &&
    last.content == "{"
  ) {
    secondToLast.content = " {";
    secondToLast.type = "openCurlyBrace";
    this->SetContentComputeType("\n", last);
    return this->AddWordToTransformed(" ");
  }
  if (
    fifthToLast.content == ")" &&
    fourthToLast.type == "whiteSpace" &&
    thirdToLast.content == "const" &&
    secondToLast.content == "\n" &&
    last.content == "{"
  ) {
    thirdToLast.content = "const ";
    this->SetContentComputeType("{", secondToLast);
    this->SetContentComputeType("\n", last);
    this->AddWordToTransformed(" ");
    return true;
  }
  if (
    sixthToLast.content == ")" &&
    fifthToLast.type == "whiteSpace" &&
    fourthToLast.content == "const" &&
    thirdToLast.content == "\n" &&
    secondToLast.type == "whiteSpace" &&
    last.content == "{"
  ) {
    fourthToLast.content = "const ";
    this->SetContentComputeType("{", thirdToLast);
    last.content = secondToLast.content + " ";
    last.type = "whiteSpace";
    this->SetContentComputeType("\n", secondToLast);
    return true;
  }
  if (secondToLast.content == ")" && last.content == "const") {
    this->SetContentComputeType(" ", last);
    this->AddWordToTransformed("const");
    return true;
  }
  return false;
}

bool CodeFormatter::ComputeState(int maximumElementsToProcess) {
  int lineLength = 0;
  int tooManyElementCounter = 0;
  for (int i = this->transformedElements.size - 1; i >= 0; i --) {
    tooManyElementCounter ++;
    if (tooManyElementCounter > maximumElementsToProcess) {
      return false;
    }
    CodeElement& current = this->transformedElements[i];
    if (current.content == "\n" || current.type == "newLine") {
      this->currentLineLength = lineLength;
      return true;
    }
    lineLength += current.content.size();
  }
  this->currentLineLength = lineLength;
  return true;
}

bool CodeFormatter::AddAndAccount(const CodeElement& incoming) {
  this->transformedElements.addOnTop(incoming);
  if (incoming.content == "\n") {
    this->previousLineIndent = this->currentLineIndent;
    this->previousLineLength = this->currentLineLength;
    this->currentLineLength = 0;
    this->currentLineIndent = 0;
  } else {
    if (this->currentLineIndent == this->currentLineLength) {
      if (incoming.type == "whiteSpace" || incoming.content == " ") {
        this->currentLineIndent += incoming.content.size();
      }
    }
    this->currentLineLength += incoming.content.size();
  }
  return true;
}

bool CodeFormatter::ApplyFormattingRules(std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::ApplyFormattingRules");
  this->transformedElements.setExpectedSize(this->originalElements.size);
  CodeElement empty;
  this->transformedElements.initializeFillInObject(6, empty);
  for (int i = 0; i < this->originalElements.size; i ++) {
    CodeElement& currentElement = this->originalElements[i];
    this->AddAndAccount(currentElement);
    int tooManyRulesCounter = 0;
    int maximumRules = 200;
    while (this->ApplyOneRule(comments)) {
      this->ComputeState(200);
      tooManyRulesCounter ++;
      if (tooManyRulesCounter > maximumRules) {
        if (comments != nullptr) {
          *comments << "Too many rules, this must be a programming error. ";
        }
        //return true;
        //int fixme;
        return false;
      }
    }
  }
  return true;
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
  this->builtInTypes.setKeyValue(" ", "whiteSpace");
  this->builtInTypes.setKeyValue("\t", "whiteSpace");
}

bool CodeFormatter::WriteFormatedCode(std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::WriteFormatedCode");
  std::fstream fileOut;
  if (!FileOperations::openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(
    fileOut, this->outputFileName, false, true, false
  )) {
    if (comments != nullptr) {
      *comments << "Failed to open source code formatting output file. ";
    }
    return false;
  }
  for (int i = 0; i < this->transformedElements.size; i ++) {
    fileOut << this->transformedElements[i].content;
  }
  return true;
}
