#include "vpfHeader1General7FileOperations_Encodings.h"
#include "source_code_formatter.h"

static ProjectInformationInstance ProjectInfoVpfSourceCodeFormatter(__FILE__, "Source code formatter implementation.");

bool SourceCodeFormatter::FormatCPPSourceCode(const std::string& fileName, std::stringstream* comments) {
  if (comments != 0) {
    *comments << "Not implemented yet. ";
  }
  SourceCodeFormatter formatter;
  formatter.inputFileName = fileName;
  std::fstream fileIn, fileOut;
  if (! FileOperations::LoadFileToStringVirtual(fileName, this->inputFileName, false, false, comments)) {
    if (comments != 0) {
      *comments << "Failed to load file. ";
    }
    return false;
  }
  if (!fileIn.is_open()) {
    if (comments != 0) {
      *comments << "Cannot open file for code formatting. ";
    }
    return false;
  }
  fileIn.clear(std::ios::goodbit);
  fileIn.seekg(0, std::ios_base::end);
  int theSize = fileIn.tellg();
  fileIn.seekg(0);
  List<char> theBuffer;
  theBuffer.SetSize(theSize * 2 + 1);
  fileIn.read(theBuffer.TheObjects, theSize * 2);
  this->outputFileName = this->inputFileName;
  this->outputFileName.append(".new");
  FileOperations::OpenFileCreateIfNotPresentVirtual(fileOut, this->outputFileName, false, true, false);
  for (int i = 0; i < theSize; i ++) {
    char lookAhead = (i < theSize - 1) ? theBuffer[i + 1] : ' ';
    switch(theBuffer[i]) {
    case '\t':
        fileOut << "  ";
        break;
      case ',':
        fileOut << theBuffer[i];
        if (lookAhead != ' ' && lookAhead != '\n' && lookAhead != '\'')
          fileOut << " ";
        break;
      case ';':
        fileOut << theBuffer[i];
        if (lookAhead != ' ' && lookAhead != '\n' && lookAhead != '\'')
          fileOut << " ";
        break;
      default:
        fileOut << theBuffer[i];
        break;
    }
  }
  return true;
}
