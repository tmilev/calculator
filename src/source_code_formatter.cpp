#include "vpfHeader1General7FileOperations_Encodings.h"
#include "source_code_formatter.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"

static ProjectInformationInstance ProjectInfoVpfSourceCodeFormatter(__FILE__, "Source code formatter implementation.");

bool CodeFormatter::initializeFileNames(
  const std::string& fileName,
  const std::string& inputOutputFileNameEmptyForAuto,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::initializeFileNames");
  this->inputFileName = fileName;
  if (!FileOperations::LoadFileToStringVirtual(this->inputFileName, this->inputCode, false, false, comments)) {
    if (comments != 0) {
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
    if (comments != 0) {
      *comments << "Format cpp directory: empty directory name not allowed. ";
    }
    return false;
  }
  if (directory[directory.size() - 1] != '/') {
    directory += "/";
  }
  List<std::string> allFiles, newFileNames, oldFileNames, allFileExtensions;
  if (!FileOperations::GetFolderFileNamesVirtual(directory, allFiles, &allFileExtensions, false, false, comments)) {
    return false;
  }
  newFileNames.SetExpectedSize(allFiles.size);
  oldFileNames.SetExpectedSize(allFiles.size);
  for (int i = 0; i < allFiles.size; i ++) {
    if (allFileExtensions[i] == ".cpp" || allFileExtensions[i] == ".h") {
      oldFileNames.AddOnTop(directory + allFiles[i]);
      newFileNames.AddOnTop(directory + "new/" + allFiles[i]);
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
  if (comments != 0) {
    (*comments) << this->ToStringLinks();
  }
  return true;
}

void CodeFormatter::AddCurrentWord() {
  this->AddWord(this->currentWord);
  this->currentWord = "";
}

void CodeFormatter::AddWord(const std::string& incomingString) {
  if (incomingString == "") {
    return;
  }
  CodeElement incoming;
  this->SetContentComputeType(incomingString, incoming);
  this->originalElements.AddOnTop(incoming);
}

bool CodeFormatter::isSeparatorCharacter(char input) {
  unsigned char inputUnsigned = input;
  return this->separatorCharactersMap[inputUnsigned];
}

bool CodeFormatter::ProcessSeparatorCharacters() {
  if (this->isSeparatorCharacter(this->currentChar)) {
    this->AddCurrentWord();
    this->currentWord = this->currentChar;
    this->AddCurrentWord();
    return true;
  }
  this->currentWord += this->currentChar;
  return true;
}

bool CodeFormatter::ProcessCharacterInQuotes() {
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
  if (this->builtInTypes.Contains(input)) {
    output.type = this->builtInTypes.GetValueConstCrashIfNotPresent(input);
  }
}

bool CodeFormatter::ExtractCodeElements(std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::ExtractCodeElements");
  (void) comments;
  this->originalElements.SetExpectedSize(this->inputCode.size());
  this->flagInQuotes = false;
  this->flagPreviousIsStandaloneBackSlash = false;
  for (unsigned i = 0; i < this->inputCode.size(); i ++) {
    this->currentChar = this->inputCode[i];
    if (this->ProcessCharacterInQuotes()) {
      continue;
    }
    if (this->ProcessSeparatorCharacters()) {
      continue;
    }
  }
  this->AddCurrentWord();
  return true;
}

bool CodeFormatter::DecreaseStack(int numberToPop) {
  this->transformedElements.SetSize(this->transformedElements.size - numberToPop);
  return true;
}

bool CodeFormatter::ApplyOneRule() {
  MacroRegisterFunctionWithName("SourceCodeFormatter::ApplyOneRule");
  int lastIndex = this->transformedElements.size - 1;
  CodeElement& last = this->transformedElements[lastIndex];
  CodeElement& secondToLast = this->transformedElements[lastIndex - 1];
  CodeElement& thirdToLast = this->transformedElements[lastIndex - 2];
  CodeElement& fourthToLast = this->transformedElements[lastIndex - 3];
  CodeElement& fifthToLast = this->transformedElements[lastIndex - 4];
  CodeElement& sixthToLast = this->transformedElements[lastIndex - 5];
  if (last.type == "whiteSpace" && secondToLast.type == "whiteSpace") {
    secondToLast.content = secondToLast.content + last.content;
    return this->DecreaseStack(1);
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
    this->AddWord(" ");
    return true;
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
    this->AddWord(" ");
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
    this->AddWord("const");
    return true;
  }
  return false;
}

bool CodeFormatter::ApplyFormattingRules(std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::ApplyFormattingRules");
  this->transformedElements.SetExpectedSize(this->originalElements.size);
  CodeElement empty;
  this->transformedElements.initializeFillInObject(6, empty);
  for (int i = 0; i < this->originalElements.size; i ++) {
    this->transformedElements.AddOnTop(this->originalElements[i]);
    int tooManyRulesCounter = 0;
    int maximumRules = 200;
    while (this->ApplyOneRule()) {
      tooManyRulesCounter ++;
      if (tooManyRulesCounter > maximumRules) {
        if (comments != 0) {
          *comments << "Too many rules, this must be a programming error. ";
        }
        return false;
      }
    }
  }
  return true;
}

CodeFormatter::CodeFormatter() {
  this->flagInQuotes = false;
  this->flagPreviousIsStandaloneBackSlash= false;
  this->currentChar = 0;
  this->separatorCharacters = "(){} \t\n\r,:;.*&+-/[]";
  this->separatorCharactersMap.initializeFillInObject(256, false);
  for (unsigned i = 0; i < this->separatorCharacters.size(); i ++) {
    this->separatorCharactersMap[(unsigned) this->separatorCharacters[i]] = true;
  }
  this->builtInTypes.SetKeyValue(" ", "whiteSpace");
  this->builtInTypes.SetKeyValue("\t", "whiteSpace");
}

bool CodeFormatter::WriteFormatedCode(std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::WriteFormatedCode");
  std::fstream fileOut;
  if (!FileOperations::OpenFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(
    fileOut, this->outputFileName, false, true, false
  )) {
    if (comments != 0) {
      *comments << "Failed to open source code formatting output file. ";
    }
    return false;
  }
  for (int i = 0; i < this->transformedElements.size; i ++) {
    fileOut << this->transformedElements[i].content;
  }
  return true;
}
