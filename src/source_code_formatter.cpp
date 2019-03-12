#include "vpfHeader1General7FileOperations_Encodings.h"
#include "source_code_formatter.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"

static ProjectInformationInstance ProjectInfoVpfSourceCodeFormatter(__FILE__, "Source code formatter implementation.");

bool SourceCodeFormatter::initializeFileNames(const std::string& fileName, std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::initializeFileNames");
  this->inputFileName = fileName;
  if (!FileOperations::LoadFileToStringVirtual(this->inputFileName, this->inputCode, false, false, comments)) {
    if (comments != 0) {
      *comments << "Failed to load file. ";
    }
    return false;
  }
  this->outputFileName = this->inputFileName;
  this->outputFileName.append(".new");
  return true;
}

std::string SourceCodeFormatter::ToStringLinks() {
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

bool SourceCodeFormatter::FormatCPPSourceCode(const std::string& fileName, std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::FormatCPPSourceCode");
  if (!this->initializeFileNames(fileName, comments)) {
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
    *comments << this->ToStringLinks();
  }
  return true;
}

void SourceCodeFormatter::AddCurrentWord() {
  if (this->currentWord.size() == 0) {
    return;
  }
  this->originalElements.AddOnTop(this->currentWord);
  this->currentWord = "";
}

bool SourceCodeFormatter::isSeparatorCharacter(char input) {
  return this->separatorCharactersMap[(unsigned) input];
}

bool SourceCodeFormatter::ProcessSeparatorCharacters() {
  if (this->isSeparatorCharacter(this->currentChar)) {
    this->AddCurrentWord();
    this->currentWord = this->currentChar;
    this->AddCurrentWord();
    return true;
  }
  this->currentWord += this->currentChar;
  return true;
}

bool SourceCodeFormatter::ProcessCharacterInQuotes() {
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

bool SourceCodeFormatter::ExtractCodeElements(std::stringstream* comments) {
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

bool SourceCodeFormatter::isWhiteSpaceNoNewLine(const std::string& input) {
  for (unsigned i = 0; i < input.size(); i ++) {
    if (!this->whiteSpaceCharacterNoNewLineMap[(unsigned) input[i]]) {
      return false;
    }
  }
  return true;
}

bool SourceCodeFormatter::DecreaseStack(int numberToPop) {
  this->transformedElements.SetSize(this->transformedElements.size - numberToPop);
  return true;
}

bool SourceCodeFormatter::ApplyOneRule() {
  MacroRegisterFunctionWithName("SourceCodeFormatter::ApplyOneRule");
  int lastIndex = this->transformedElements.size - 1;
  std::string& last = this->transformedElements[lastIndex];
  std::string& secondToLast = this->transformedElements[lastIndex - 1];
  std::string& thirdToLast = this->transformedElements[lastIndex - 2];
  std::string& fourthToLast = this->transformedElements[lastIndex - 3];
  //std::string& fifthToLast = this->transformedElements[lastIndex - 4];
  if (this->isWhiteSpaceNoNewLine(last) && this->isWhiteSpaceNoNewLine(secondToLast)) {
    secondToLast = secondToLast + last;
    return this->DecreaseStack(1);
  }
  if (
    fourthToLast == ")" &&
    thirdToLast == "\n" &&
    this->isWhiteSpaceNoNewLine(secondToLast) &&
    last == "{"
  ) {
    thirdToLast = " {\n";
    secondToLast = secondToLast + " ";
    return this->DecreaseStack(1);
  }
  if (
    thirdToLast == ")" &&
    secondToLast == "\n" &&
    last == "{"
  ) {
    secondToLast = " {\n";
    last = " ";
    return true;
  }
  return false;
}

bool SourceCodeFormatter::ApplyFormattingRules(std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::ApplyFormattingRules");
  this->transformedElements.SetExpectedSize(this->originalElements.size);
  this->transformedElements.initializeFillInObject(6, "");
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

SourceCodeFormatter::SourceCodeFormatter() {
  this->flagInQuotes = false;
  this->flagPreviousIsStandaloneBackSlash= false;
  this->currentChar = 0;
  this->separatorCharacters = "(){} \t\n\r,:;.*&+-/[]";
  this->separatorCharactersMap.initializeFillInObject(256, false);
  for (unsigned i = 0; i < this->separatorCharacters.size(); i ++) {
    this->separatorCharactersMap[(unsigned) this->separatorCharacters[i]] = true;
  }
  this->whiteSpaceCharactersNoNewLine = " \t\r";
  this->whiteSpaceCharacterNoNewLineMap.initializeFillInObject(256, false);
  for (unsigned i = 0; i < this->whiteSpaceCharactersNoNewLine.size(); i ++) {
    this->whiteSpaceCharacterNoNewLineMap[(unsigned) this->whiteSpaceCharactersNoNewLine[i]] = true;
  }
}

bool SourceCodeFormatter::WriteFormatedCode(std::stringstream* comments) {
  MacroRegisterFunctionWithName("SourceCodeFormatter::WriteFormatedCode");
  std::fstream fileOut;
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(fileOut, this->outputFileName, false, true, false)) {
    if (comments != 0) {
      *comments << "Failed to open source code formatting output file. ";
    }
    return false;
  }
  for (int i = 0; i < this->transformedElements.size; i ++) {
    fileOut << this->transformedElements[i];
  }
  return true;
}
