#include "vpfHeader1General7FileOperations_Encodings.h"
#include "source_code_formatter.h"

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

bool SourceCodeFormatter::FormatCPPSourceCode(const std::string& fileName, std::stringstream* comments) {
  if (!this->initializeFileNames(fileName, comments)) {
    return false;
  }
  std::fstream fileOut;
  FileOperations::OpenFileCreateIfNotPresentVirtual(fileOut, this->outputFileName, false, true, false);
  for (unsigned i = 0; i < this->inputCode.size(); i ++) {
    fileOut << this->inputCode[i];
  }

  return true;
}
