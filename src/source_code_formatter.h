// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_source_code_formatter_ALREADY_INCLUDED
#define header_source_code_formatter_ALREADY_INCLUDED
#include "macros.h"
#include "general_list_references.h"
#include "general_maps.h"

class CodeElement {
public:
  std::string content;
  int indentLevel;
  std::string toString();
  CodeElement();
};

class CodeFormatter {
public:
  std::string inputFileName;
  std::string outputFileName;
  bool flagInQuotes;
  bool flagPreviousIsStandaloneBackSlash;
  std::string inputCode;
  char currentChar;
  std::string currentWord;
  List<CodeElement> originalElements;
  List<CodeElement> stack;
  std::string transformedContent;
  std::string separatorCharacters;
  List<bool> separatorCharactersMap;
  int maximumDesiredLineLength;
  int previousLineIndent;
  int previousLineLength;
  int currentLineIndent;
  int currentLineLength;
  CodeFormatter();
  void addCurrentWord();
  static bool formatCPPDirectory(const std::string& inputDirectory, std::stringstream* comments);
  bool formatCPPSourceCode(
    const std::string& inputFileName,
    const std::string& inputOutputFileNameEmptyForAuto,
    std::stringstream* comments
  );
  bool processCharacterInQuotes();
  bool isSeparatorCharacter(char input);
  bool processSeparatorCharacters();
  bool extractCodeElements(std::stringstream* comments);
  bool format(std::stringstream* comments);
  bool writeFormatedCode(std::stringstream* comments);
  std::string toStringTransformed6();
  bool computeState(int maximumElementsToProcess);
  bool addAndAccount(const CodeElement& incoming);
  bool decreaseStack(int numberToPop);
  bool initializeFileNames(
    const std::string& fileName,
    const std::string& inputOutputFileNameEmptyForAuto,
    std::stringstream* comments
  );
  std::string toStringLinks();
};

#endif // header_source_code_formatter_ALREADY_INCLUDED
