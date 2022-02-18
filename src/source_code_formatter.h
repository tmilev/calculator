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
  public:
    enum Type {
      Dummy,
      Unknown,
      IncludeLineStart,
      IncludeLine,
      MacroLineStart,
      MacroLine,
      Comment,
      CommentMultiline,
      CommentCollection,
      EndLine,
      Quote,
      SingleQuote,
      ConstKeyWord,
      StaticKeyWord,
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
      // An identifier such as f::y.
      Identifier,
      // A user-defined atom such as foo or bar.
      Atom,
      ControlWantsCodeBlock,
      Colon,
      SemiColon,
      DoubleColon,
      InParentheses,
      InBrackets,
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
      FunctionDefinition,
      Ampersand,
      Star,
      CaseKeyWord,
      DefaultKeyWord,
      CaseClauseStart,
      CaseClauseMultipleStart,
      CaseClause,
      CaseClauseList,
      ClassKeyWord,
      ClassDeclaration,
      ClassDefinition,
      EnumKeyWord,
      EnumDeclaration,
      EnumDefinition,
      PublicKeyWord,
      PrivateKeyWord,
      VisibilityClause,
      Exclamation,
      If,
      IfWantsCodeBlock,
      IfClause,
      Else,
      Return,
      ReturnedExpression,
    };
  private:
    void computeIndentation();
    void computeIndentationFunctionDeclaration();
    void computeIndentationFunctionDefinition();
    void computeIndentationCodeBlock();
    void computeIndentationControlWantsCodeBlock();
    void computeIndentationCommandList();
    void computeIndentationCommaList();
    void computeIndentationCommand();
    void computeIndentationComment();
    void computeIndentationCommentCollection();
    void computeIndentationCurlyBraceCommaDelimitedList();
    void computeIndentationTypeExpression();
    void computeIndentationReturnedExpression();
    void computeIndentationOperator();
    void computeIndentationInParentheses();
    void computeIndentationBasic(int startingIndex);
    void computeIndentationExpression();
    void computeIndentationTopLevel();
    void computeIndentationAtomic();
    bool applyOpenParenthesisException();
    void computeIndentationGreaterThan();
    void computeIndentationLessThan();
    void computeIndentationCaseClause();
    void computeIndentationCaseClauseStart();
    void computeIndentationIfClause();
    void formatDefault(std::stringstream& out);
    void formatContent(std::stringstream& out);
    bool needsWhiteSpaceBefore();
    bool containsNewLineAfterRecursively();
    void resetWhitespaceRecursively();
    int offsetFromPrevious();
    int minimalSizeWithSpacebars();
    bool isExpressionLikeForIndentation();
    // Breaks lines in a long expression.
    void breakExpression();
    // A special break-line rune for stringstreams, i.e.,
    // expressions of the form:
    // out << something1 << something2 << ...
    // Returns false if the expression is not of that form.
    bool breakExpressionStringStream();
    bool isStringStreamExpressionLike();
  public:
    Element::Type type;
    std::string content;
    List<CodeFormatter::Element> children;
    CodeFormatter* owner;
    Element* parent;
    int indexInParent;
    // The indentation level of the containing code block.
    int indentationLevel;
    int whiteSpaceBefore;
    // The number of columns needed to display the previous
    // code element.
    int columnFinal;
    int newLinesAfter;
    bool requiresWhiteSpaceAfter;
    std::string toStringContentAndMetaData() const;
    std::string toStringIndentation() const;
    std::string toString() const;
    std::string toStringWithoutType() const;
    static std::string toStringType(CodeFormatter::Element::Type inputType);
    std::string toStringHTMLTree();
    void toStringContentOnly(std::stringstream& out) const;
    std::string toStringContentOnly() const;
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
    bool isSuitableForTemplateArgument() const;
    bool isSuitableForParenthesesEnclosure() const;
    bool isSuitableForCommaListElement() const;
    bool isRightDelimiter() const;
    bool isIdentifierOrAtom() const;
    bool isExpressionOrAtom() const;
    bool isExpressionIdentifierOrAtom() const;
    bool isExpressionIdentifierAtomOrFunctionWithArguments() const;
    bool isCommandListOrCommand() const;
    bool isParenthesesBlock() const;
    bool isCodeBlock() const;
    bool isCodeBlockOrCommand() const;
    bool isComment() const;
    bool isTypeWord() const;
    bool isTypeWordOrTypeExpression() const;
    bool isTypeOrIdentifierOrExpression() const;
    bool isStarOrAmpersand() const;
    bool canBeUnaryOnTheRight() const;
    bool canBeUnaryOnTheLeft() const;
    bool isOperator() const;
    bool isOperatorOrInequality() const;
    bool isColonDoubleColonOrComma() const;
    bool isColonOrDoubleColon() const;
    void initializePointers(
      CodeFormatter* inputOwner,
      CodeFormatter::Element* inputParent,
      int inputIndexInParent
    );
    Element* previousAtom();
    Element* previousSibling();
    Element* rightMostAtomUnderMe();
    Element* leftMostAtomUnderMe();
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
    void makeFrom5(
      CodeFormatter::Element::Type inputType,
      const Element& leftmost,
      const Element& secondToLeft,
      const Element& middle,
    const Element& secondToRight,
    const Element& right
    );
    std::string format();
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
    bool isSuitableForExpressionOperatorExpression(CodeFormatter::Element& left,
    CodeFormatter::Element& middle,
    CodeFormatter::Element& right
    , Element &lookAhead);
    bool isSuitableForUnaryOperatorExpression(CodeFormatter::Element& first,
    CodeFormatter::Element& unary,
    CodeFormatter::Element& expression
    , Element &lookAhead);
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
  HashedList<std::string, HashFunctions::hashFunction> needsSpaceToTheRightContainer;
  // Keywords such as int, boo, etc.
  HashedList<std::string, HashFunctions::hashFunction> typeKeyWords;
  // Keywords such as for, if, etc that take as input an in-parentheses block and an
  // in-curly-brace block.
  HashedList<std::string, HashFunctions::hashFunction> controlKeyWords;
  MapList<std::string, CodeFormatter::Element::Type, HashFunctions::hashFunction> elementTypes;
  // A list of elements interpreted as operators.
  HashedList<std::string, HashFunctions::hashFunction> operatorList;

  // A list of operator overrides.
  // An operator K given in the key overrides any operator V given in the values.
  // In other words, the expression
  // xVyKz is to be interpreted as xV(yKz) rather than as (xVy)Kz
  MapList<std::string, HashedList<std::string, HashFunctions::hashFunction>, HashFunctions::hashFunction>
  operatorOverrides;

  int maximumDesiredLineLength;
  int indexCurrentlyConsumed;
  const int tabLength = 2;
  const int dummyElements = 4;
  // A class to represent
  CodeFormatter();
  static bool formatCPPDirectory(const std::string& inputDirectory, std::stringstream* comments);
  bool formatCPPSourceCode(const std::string& inputFileName,
    const std::string& inputOutputFileNameEmptyForAuto,
    std::stringstream* comments
  , bool logDebugInfo);
private:
  void addOperatorOverride(const std::string& overridingOperator, const List<std::string>& overridden);
  bool shouldSeparateWithSpace(const std::string& left, const std::string& right);
  bool preemptsWhitespaceBefore(char input);
  bool needsSpaceToTheRight(const std::string& word);
  bool isSeparatorCharacter(char input);
  bool isSeparator(const std::string& input);
  bool isOperator(const std::string& input);
  bool isTypeKeyWord(const std::string& input);
  bool isControlKeyWord(const std::string& input);
  bool isWhiteSpace(const std::string& input);
  bool writeFormatedCode(std::stringstream* comments);
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
  bool isLetterLike(char input);
  void wirePointersRecursively(
    CodeFormatter::Element& current,
    CodeFormatter::Element* parent,
    int indexInParent
  );
  bool isOperatorSuitableForNormalization(const CodeFormatter::Element& element);
  void normalizeBinaryOperationsRecursively(
    CodeFormatter::Element& current
  );
  void correctMultiArguments(CodeFormatter::Element& inputOutput);
  void collectMultiArguments(
  const std::string& operatorName,
    CodeFormatter::Element& current,
  List<CodeFormatter::Element>& output
  );
  bool rightOperatorOverridesLeft(
  const CodeFormatter::Element& leftOperator,
  const CodeFormatter::Element& rightOperator
  );

};

#endif // header_source_code_formatter_ALREADY_INCLUDED
