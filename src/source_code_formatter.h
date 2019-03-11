//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderHtmlSourceCodeFormatter
#define vpfHeaderHtmlSourceCodeFormatter
#include "vpfMacros.h"
#include "vpfHeader1General1_ListReferences.h"
#include "vpfHeader1General6Maps.h"

static ProjectInformationInstance ProjectInfoHeaderSourceCodeFormatterHeader(__FILE__, "Source code formatter header.");

class SourceCodeFormatter {
public:
  std::string inputFileName, outputFileName;
  bool flagInQuotes;
  bool flagPreviousIsStandaloneBackSlash;
  std::string inputCode, outputCode;
  char currentChar;
  std::string currentWord;
  List<std::string> codeElements;
  std::string separatorCharacters;
  List<bool> separatorCharacterMap;
  SourceCodeFormatter();
  void AddCurrentWord();
  bool FormatCPPSourceCode(const std::string& fileName, std::stringstream* comments);
  bool ProcessCharacterInQuotes();
  bool ProcessSeparatorCharacters();
  bool ExtractCodeElements(std::stringstream* comments);
  bool WriteFormatedCode(std::stringstream* comments);
  bool initializeFileNames(const std::string& fileName, std::stringstream* comments);
  std::string ToStringLinks();
};
#endif
