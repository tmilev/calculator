#include "vpfHeader1General7FileOperations_Encodings.h"
#include "source_code_formatter.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"

static ProjectInformationInstance ProjectInfoVpfSourceCodeFormatter(__FILE__, "Source code formatter implementation.");

bool SourceCodeFormatter::initializeFileNames(const std::string& fileName, std::stringstream* comments) {
  if (comments != 0) {
    *comments << "Not implemented yet. ";
  }
  this->inputFileName = fileName;
  if (! FileOperations::LoadFileToStringVirtual(this->inputFileName, this->inputCode, false, false, comments)) {
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
  if (!this->initializeFileNames(fileName, comments)) {
    return false;
  }
  if (!this->ExtractCodeElements(comments)) {
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
  this->codeElements.AddOnTop(this->currentWord);
  this->currentWord = "";
}

bool SourceCodeFormatter::ProcessSeparatorCharacters() {
  if (this->separatorCharacterMap[this->currentChar]) {
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
  (void) comments;
  this->codeElements.SetExpectedSize(this->inputCode.size());
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

SourceCodeFormatter::SourceCodeFormatter() {
  this->flagInQuotes = false;
  this->flagPreviousIsStandaloneBackSlash= false;
  this->currentChar = 0;
  this->separatorCharacters = "(){} \t\n,:;.*&+-/[]";
  this->separatorCharacterMap.initializeFillInObject(256, false);
  for (unsigned i = 0; i < this->separatorCharacters.size(); i ++) {
    this->separatorCharacterMap[this->separatorCharacters[i]] = true;
  }
}

bool SourceCodeFormatter::WriteFormatedCode(std::stringstream* comments) {
  std::fstream fileOut;
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(fileOut, this->outputFileName, false, true, false)) {
    if (comments != 0) {
      *comments << "Failed to open source code formatting output file. ";
    }
    return false;
  }
  for (int i = 0; i < this->codeElements.size; i ++) {
    fileOut << this->codeElements[i];
  }
  return true;
}
