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
  std::string inputFileName;
  std::string outputFileName;
  bool flagInQuotes;
  bool flagPreviousIsStandaloneBackSlash;
  std::string inputCode, outputCode;
  char currentChar;
  std::string currentWord;
  List<std::string> originalElements;
  List<std::string> transformedElements;
  std::string whiteSpaceCharactersNoNewLine;
  std::string separatorCharacters;
  List<bool> separatorCharactersMap;
  List<bool> whiteSpaceCharacterNoNewLineMap;
  SourceCodeFormatter();
  void AddCurrentWord();
  static bool FormatCPPDirectory(const std::string& inputDirectory, std::stringstream* comments);
  bool FormatCPPSourceCode(
    const std::string& inputFileName,
    const std::string& inputOutputFileNameEmptyForAuto,
    std::stringstream* comments
  );
  bool ProcessCharacterInQuotes();
  bool isSeparatorCharacter(char input);
  bool ProcessSeparatorCharacters();
  bool ExtractCodeElements(std::stringstream* comments);
  bool WriteFormatedCode(std::stringstream* comments);
  bool ApplyFormattingRules(std::stringstream* comments);
  bool ApplyOneRule();
  bool DecreaseStack(int numberToPop);
  bool isWhiteSpaceNoNewLine(const std::string& input);
  bool initializeFileNames(
    const std::string& fileName,
    const std::string& inputOutputFileNameEmptyForAuto,
    std::stringstream* comments
  );
  std::string ToStringLinks();
};
#endif
