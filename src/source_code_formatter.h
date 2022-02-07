// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_source_code_formatter_ALREADY_INCLUDED
#define header_source_code_formatter_ALREADY_INCLUDED
#include "macros.h"
#include "general_list_references.h"
#include "general_maps.h"

class CodeFormatter {
public:
  class Element {
  private:
    void computeIndentation();
    void computeIndentationCodeBlock();
    void computeIndentationTypeExpression();
    void computeIndentationOperator();
    void computeIndentationBasic(int startingIndex);
    void computeIndentationBasicIgnorePrevious(int startingIndex);
    void computeIndentationTopLevel();
    void computeIndentationAtomic();
    void formatDefault(std::stringstream& out);
    void formatContent(std::stringstream& out);
    void setOwnerRecursively(CodeFormatter* inputOwner);
  public:
    enum Type {
      Dummy,
      Unknown,
      IncludeLineStart,
      IncludeLine,
      Comment,
      CommentMultiline,
      EndLine,
      Quote,
      SingleQuote,
      ConstKeyWord,
      ControlKeyWord,
      ConstructorExternal,
      TypeKeyWord,
      // Type+identifer as in:
      // int x
      // or
      // MyString x
      // or
      // MyString x,y
      TypeAndIdentifier,
      // An expression that describes a type such as:
      // const int, int, const int&.
      TypeExpression,
      // A computed expression, such as this->x or x+2.
      Expression,
      // An (incomplete) expression ending with an operator such as:
      // x+
      // or
      // x==
      ExpressionEndingWithOperator,
      // An identifier such as f::y.
      Identifier,
      // A user-defined atom such as foo or bar.
      Atom,
      ControlWantsCodeBlock,
      Colon,
      SemiColon,
      DoubleColon,
      InParentheses,
      LeftParenthesis,
      RightParenthesis,
      LeftBracket,
      RightBracket,
      LeftCurlyBrace,
      RightCurlyBrace,
      // A comma-delimited list surrounded by curly braces such as
      // {1,2,3}.
      CurlyBraceCommaDelimitedList,
      CodeBlock,
      TopLevel,
      FileContent,
      Operator,
      Command,
      CommandList,
      Comma,
      CommaList,
      GreaterThan,
      LessThan,
      FunctionWithArguments,
      FunctionDeclaration,
      Ampersand,
      Star,
      CaseKeyWord,
      DefaultKeyWord,
      CaseClauseStart,
      CaseClause,
      Exclamation,
      If,
      IfWantsCodeBlock,
      IfClause,
      Else,
      Return,
      ReturnedExpression,
    };
    Element::Type type;
    std::string content;
    List<CodeFormatter::Element> children;
    CodeFormatter* owner;
    // The indentation level of the containing code block.
    int indentCodeBlock;
    int whiteSpaceBefore;
    // The number of columns needed to display the previous
    // code element.
    int columnPreviousElement;
    int columnFinal;
    int newLinesAfter;
    int newLinesBefore;
    bool previousRequiresSeparatorAfter;
    bool previousRequiresWhiteSpaceAfter;
    bool requiresSeparatorAfter;
    bool requiresWhiteSpaceAfter;
    bool requiresSeparatorBefore;
    std::string toStringFormattingData() const;
    std::string toString() const;
    std::string toStringWithoutType() const;
    static std::string toStringType(CodeFormatter::Element::Type inputType);
    Element();
    void clear();
    void appendType(const CodeFormatter::Element& other, CodeFormatter::Element::Type inputType);
    void appendExpression(const CodeFormatter::Element& other);
    void appendIdentifier(const CodeFormatter::Element& other);
    void appendCommand(const CodeFormatter::Element& other);
    void addChild(const CodeFormatter::Element& other);
    bool isSuitableForCommand() const;
    bool isSuitableForTopLevel() const;
    bool isSuitableForTypeExpression() const;
    bool isSuitableForParenthesesEnclosure() const;
    bool isSuitableForCommaListElement() const;
    bool isRightDelimiter() const;
    bool isIdentifierOrAtom() const;
    bool isExpressionOrAtom() const;
    bool isExpressionIdentifierOrAtom() const;
    bool isCommandListOrCommandOrClause() const;
    bool isCommandOrClause() const;
    bool isParenthesesBlock() const;
    bool isCodeBlock() const;
    bool isCodeBlockOrCommand() const;
    bool isTypeWord() const;
    bool isTypeWordOrTypeExpression() const;
    bool isTypeOrIdentifierOrExpression() const;
    bool isStarOrAmpersand() const;
    bool isOperator() const;
    bool isColonDoubleColonOrComma() const;
    bool isColonOrDoubleColon() const;
    void makeFrom1(
      CodeFormatter::Element::Type inputType,
      const Element& child
    );
    void makeFrom2(
      CodeFormatter::Element::Type inputType,
      const Element& left,
      const Element& right
    );
    void makeFrom3(
      CodeFormatter::Element::Type inputType,
      const Element& left,
      const Element& middle,
      const Element& right
    );
    void makeFrom4(
      CodeFormatter::Element::Type inputType,
      const Element& first,
      const Element& second,
      const Element& third,
      const Element& fourth
    );
    std::string format(CodeFormatter& owner);
  };

  class Words {
  public:
    bool flagPreviousIsStandaloneBackSlash;
    CodeFormatter* owner;
    char currentChar;
    CodeFormatter::Element currentWord;
    List<CodeFormatter::Element> elements;
    void initialize(CodeFormatter& inputOwner);
    bool processSeparatorCharacters();
    bool processCharacterInQuotes();
    bool processCharacterInSingleQuotes();
    bool processCharacterInComment();
    bool processCharacterInMultilineComment();
    void addCurrentWord();
    bool previousIsForwardSlash();
    bool extractCodeElements(std::stringstream* comments);
    Words();
  };

  class Processor {
  public:
    CodeFormatter* owner;
    CodeFormatter::Element code;
    List<CodeFormatter::Element> stack;
    std::string lastRuleName;
    std::string debugLog;
    bool flagPrepareReport;
    bool flagExceededReportSize;

    void consumeElements();
    void consumeOneElement(CodeFormatter::Element& incoming);
    bool applyOneRule();
    bool removeLast();
    bool removeLast(int count);
    bool removeBelowLast(int count);
    Processor();
    void initialize(CodeFormatter& inputOwner);
    std::string toStringStack();
    void appendLog();
  };

  std::string inputFileName;
  std::string outputFileName;
  Words words;
  CodeFormatter::Processor processor;
  std::string inputCode;

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
  // Keywords such as int, boo, etc.
  HashedList<std::string, HashFunctions::hashFunction> typeKeyWords;
  // Keywords such as for, if, etc that take as input an in-parentheses block and an
  // in-curly-brace block.
  HashedList<std::string, HashFunctions::hashFunction> controlKeyWords;
  MapList<std::string, CodeFormatter::Element::Type, HashFunctions::hashFunction> elementTypes;
  // A list of elements interpreted as operators.
  HashedList<std::string, HashFunctions::hashFunction> operatorList;

  int maximumDesiredLineLength;
  int indexCurrentlyConsumed;
  const int tabLength = 2;
  const int dummyElements = 4;
  // A class to represent
  CodeFormatter();
  static bool formatCPPDirectory(const std::string& inputDirectory, std::stringstream* comments);
  bool formatCPPSourceCode(
    const std::string& inputFileName,
    const std::string& inputOutputFileNameEmptyForAuto,
    std::stringstream* comments
  );
  bool shouldSeparateWithSpace(const std::string& left, const std::string& right);
  bool isSeparatorCharacter(char input);
  bool isSeparator(const std::string& input);
  bool isOperator(const std::string& input);
  bool isTypeKeyWord(const std::string& input);
  bool isControlKeyWord(const std::string& input);
  bool isWhiteSpace(const std::string& input);
  bool writeFormatedCode(std::stringstream* comments);
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
  static bool isIdentifierWord(const std::string& input);
  static bool isIdentifierCharacter(char input);
};

#endif // header_source_code_formatter_ALREADY_INCLUDED
