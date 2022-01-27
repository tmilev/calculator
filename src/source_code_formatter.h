// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_source_code_formatter_ALREADY_INCLUDED
#define header_source_code_formatter_ALREADY_INCLUDED
#include "macros.h"
#include "general_list_references.h"
#include "general_maps.h"

class CodeElement {
public:
  int indentLevel;
  int currentLineLength = 0;
  bool isIncludeLine;
  bool isComment;
  bool isInQuote;
  std::string content;
  List<CodeElement> children;
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
  List<std::string> originalElements;
  List<CodeElement> stack;
  std::string transformedContent;
  std::string separatorCharacters;
  std::string doesntNeedSpaceToTheRight;
  std::string doesntNeedSpaceToTheLeft;
  // The preceding element to the one currently consumed, on the same line.
  // Empty string if the line was just started.
  std::string precedingElementThisLine;
  List<bool> separatorCharactersMap;
  HashedList<List<std::string> > pairsNotSeparatedBySpace;
  HashedList<std::string, HashFunctions::hashFunction> doesntNeedSpaceToTheRightContainer;
  HashedList<std::string, HashFunctions::hashFunction> doesntNeedSpaceToTheLeftContainer;
  int maximumDesiredLineLength;
  int indexCurrentlyConsumed;
  const int tabLength = 2;
  CodeFormatter();
  void addCurrentWord();
  static bool formatCPPDirectory(const std::string& inputDirectory, std::stringstream* comments);
  bool formatCPPSourceCode(
    const std::string& inputFileName,
    const std::string& inputOutputFileNameEmptyForAuto,
    std::stringstream* comments
  );
  std::string peekNext() const;
  bool processCharacterInQuotes();
  bool shouldSeparateWithSpace(const std::string& left, const std::string& right);
  bool isSeparatorCharacter(char input);
  bool isSeparator(const std::string& input);
  bool processSeparatorCharacters();
  bool extractCodeElements(std::stringstream* comments);
  bool format(std::stringstream* comments);
  void consumeElement(std::stringstream& out, const std::string& input);
  void writeElement(std::stringstream& out, const std::string& input);
  void consumeInComment(
    std::stringstream& out, const std::string& input
  );
  void consumeInQuote(
    std::stringstream& out, const std::string& input
  );
  void consumeInMultiLineComment(
    std::stringstream& out, const std::string& input
  );
  void consumeInIncludeLine(
    std::stringstream& out, const std::string& input
  );
  void breakLine(std::stringstream& out);
  void indent(std::stringstream& out);
  bool isWhiteSpace(const std::string& input);
  bool writeFormatedCode(std::stringstream* comments);
  void popStack();
  std::string toStringTransformed6();
  bool computeState(int maximumElementsToProcess);
  bool addAndAccount(const std::string& incoming);
  bool decreaseStack(int numberToPop);
  bool initializeFileNames(
    const std::string& fileName,
    const std::string& inputOutputFileNameEmptyForAuto,
    std::stringstream* comments
  );
  std::string toStringLinks();
};

#endif // header_source_code_formatter_ALREADY_INCLUDED
