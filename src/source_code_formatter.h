//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderHtmlSourceCodeFormatter
#define vpfHeaderHtmlSourceCodeFormatter
#include "vpfMacros.h"
#include "vpfHeader1General1_ListReferences.h"
#include "vpfHeader1General6Maps.h"

static ProjectInformationInstance ProjectInfoHeaderSourceCodeFormatterHeader(__FILE__, "Source code formatter header.");

class CodeElement {
public:
  std::string content;
  std::string type;
  std::string ToString();
};

class CodeFormatter {
public:
  std::string inputFileName;
  std::string outputFileName;
  bool flagInQuotes;
  bool flagPreviousIsStandaloneBackSlash;
  std::string inputCode, outputCode;
  char currentChar;
  std::string currentWord;
  MapLisT<std::string, std::string, MathRoutines::HashString> builtInTypes;
  List<CodeElement> originalElements;
  List<CodeElement> transformedElements;
  std::string whiteSpaceCharactersNoNewLine;
  std::string separatorCharacters;
  List<bool> separatorCharactersMap;
  int maximumDesiredLineLength;
  int previousLineIndent;
  int previousLineLength;
  int currentLineIndent;
  int currentLineLength;
  CodeFormatter();
  void AddCurrentWord();
  bool AddWordToTarget(const std::string& incomingString, List<CodeElement>& output);
  bool AddWordToOriginals(const std::string& incomingString);
  bool AddWordToTransformed(const std::string& incomingString);
  void SetContentComputeType(const std::string& input, CodeElement& output);
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
  bool ApplyOneRule(std::stringstream* comments);
  std::string ToStringTransformed6();
  bool ComputeState(int maximumElementsToProcess);
  bool AddAndAccount(const CodeElement& incoming);
  bool DecreaseStack(int numberToPop);
  bool initializeFileNames(
    const std::string& fileName,
    const std::string& inputOutputFileNameEmptyForAuto,
    std::stringstream* comments
  );
  std::string ToStringLinks();
};
#endif