// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
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
      EndFile,
      Quote,
      SingleQuote,
      ConstKeyWord,
      TypeAdjectiveKeyWord,
      FriendKeyWord,
      OperatorKeyWord,
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
      DoKeyWord,
      Colon,
      SemiColon,
      DoubleColon,
      InAngleBrackets,
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
      NewKeyWord,
      DeleteKeyWord,
      Command,
      CommandList,
      Comma,
      CommaList,
      GreaterThan,
      LessThan,
      Tilde,
      QuestionMark,
      FunctionWithArguments,
      FunctionWithArgumentsAndInitializerList,
      FunctionDeclaration,
      FunctionDefinition,
      Ampersand,
      Star,
      TryKeyWord,
      CatchKeyWord,
      CaseKeyWord,
      DefaultKeyWord,
      CaseClauseStart,
      CaseClauseMultipleStart,
      CaseClause,
      CaseClauseList,
      TemplateKeyWord,
      TemplateClause,
      ClassKeyWord,
      StructKeyWord,
      TypenameKeyWord,
      ClassDeclaration,
      ClassDefinition,
      EnumKeyWord,
      EnumDeclaration,
      EnumDefinition,
      VisibilityKeyWord,
      VisibilityClause,
      UnionKeyWord,
      Exclamation,
      If,
      IfWantsCodeBlock,
      IfClause,
      Else,
      Return,
      ReturnedExpression,
    };
  private:
    bool computeIndentation();
    bool computeIndentationFunctionDeclaration();
    bool computeIndentationFunctionDefinition();
    bool computeIndentationCodeBlock();
    bool computeIndentationControlWantsCodeBlock();
    bool computeIndentationCommandList();
    bool computeIndentationCommaList();
    bool computeIndentationInitializerList();
    bool computeIndentationEnumDefinition();
    bool computeIndentationEnumDeclaration();
    bool computeIndentationCommand();
    bool computeIndentationComment();
    bool computeIndentationCommentCollection();
    bool computeIndentationCurlyBraceCommaDelimitedList();
    bool computeIndentationTypeExpression();
    bool computeIndentationReturnedExpression();
    bool computeIndentationOperator();
    bool computeIndentationInParentheses();
    bool computeIndentationInBrackets();
    bool computeIndentationAngleBrackets();
    bool computeIndentationBasic(int startingIndex);
    bool computeIndentationExpression();
    bool computeIndentationTopLevel();
    bool computeIndentationTemplateClause();
    bool computeIndentationMacroline();
    void computeIndentationAtomic();
    bool computeIndentationGreaterThan();
    bool computeIndentationLessThan();
    bool computeIndentationCaseClause();
    bool computeIndentationCaseClauseMultipleStart();
    bool computeIndentationCaseClauseStart();
    bool computeIndentationCaseClauseList();
    bool computeIndentationIfClause();
    bool computeIndentationIfWantsCodeBlock();
    bool computeIndentationQuote();
    void formatDefault(std::stringstream& out);
    void formatContent(std::stringstream& out);
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
    bool isOfTypeOrCommandOfType(CodeFormatter::Element::Type input);
    bool shouldAddExtraLineInTopLevel(CodeFormatter::Element& next);
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
    std::string toStringContentAndMetaData() const;
    std::string toStringIndentation() const;
    std::string toString() const;
    std::string toStringWithoutType() const;
    static std::string toStringType(CodeFormatter::Element::Type inputType);
    std::string toStringMyType() const;
    std::string toStringHTMLTree();
    void toStringContentOnly(std::stringstream& out) const;
    std::string toStringContentOnly() const;
    Element();
    void clear();
    void appendType(
      const CodeFormatter::Element& other,
      CodeFormatter::Element::Type inputType
    );
    void appendExpression(const CodeFormatter::Element& other);
    void addChild(const CodeFormatter::Element& other);
    bool isSuitableForCommand() const;
    bool isSuitableForTopLevel() const;
    bool isSuitableForTypeExpression() const;
    bool isSuitableForTemplateArgument() const;
    bool isSuitableForParenthesesEnclosure() const;
    bool isRightDelimiter() const;
    bool isRightDelimiterOrSemicolon() const;
    bool isLeftDelimiter() const;
    bool isIdentifierOrAtom() const;
    bool isExpressionOrAtom() const;
    bool isExpressionIdentifierOrAtom() const;
    bool isExpressionIdentifierAtomOrFunctionWithArguments() const;
    bool isExpressionIdentifierAtomFunctionWithArgumentsOrInParentheses() const
    ;
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
    Element* previousAtom() const;
    Element* nextAtom() const;
    Element* previousSibling();
    Element* rightMostAtomUnderMe();
    Element* leftMostAtomUnderMe();
    void makeFrom1(
      CodeFormatter::Element::Type inputType, const Element& child
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
    CodeFormatter::Element dummy;
    std::string lastRuleName;
    std::string debugLog;
    bool flagPrepareReport;
    bool flagExceededReportSize;
    void consumeElements();
    void consumeOneElement(CodeFormatter::Element& incoming);
    bool isSuitableForExpressionOperatorExpression(
      CodeFormatter::Element& left,
      CodeFormatter::Element& operatorElement,
      CodeFormatter::Element& right,
      Element& lookAhead
    );
    bool isSuitableForExpressionOperatorExpressionXX(
      CodeFormatter::Element& left,
      CodeFormatter::Element& operatorElement,
      CodeFormatter::Element& right,
      Element& lookAheadFirst,
      Element& lookAheadNext
    );
    bool isSuitableForTypePointer(
      CodeFormatter::Element& prefix,
      CodeFormatter::Element& type,
      CodeFormatter::Element& starOrAmpersand,
      CodeFormatter::Element& suffix,
      CodeFormatter::Element& afterSuffix
    );
    bool isSuitableForExpressionExpressionToExpression(
      CodeFormatter::Element& left, CodeFormatter::Element& right
    ) const;
    bool allowTypeAndIdentifier(
      CodeFormatter::Element& prefix,
      Element& toBeTyped,
      CodeFormatter::Element& suffix
    );
    bool isSuitableForUnaryOperatorExpression(
      CodeFormatter::Element& first,
      CodeFormatter::Element& unary,
      CodeFormatter::Element& expression,
      Element& lookAhead
    );
    bool isSuitableForUnaryOperatorExpressionXX(
      CodeFormatter::Element& first,
      CodeFormatter::Element& unary,
      CodeFormatter::Element& expression,
      Element& lookAhead
    );
    bool isSuitableForFunction(CodeFormatter::Element& input);
    bool areSuitableForCommaSeparatedList(
      CodeFormatter::Element& left, CodeFormatter::Element& right
    );
    bool isSuitableForCommaSeparatedList(CodeFormatter::Element& input);
    bool isNoReturn(CodeFormatter::Element& input);
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
  std::string outputOnFailFileName;
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
  HashedList<std::string, HashFunctions::hashFunction>
  doesntNeedSpaceToTheRightContainer;
  HashedList<std::string, HashFunctions::hashFunction>
  doesntNeedSpaceToTheLeftContainer;
  HashedList<std::string, HashFunctions::hashFunction>
  needsSpaceToTheRightContainer;
  // Keywords such as int, boo, etc.
  HashedList<std::string, HashFunctions::hashFunction> typeKeyWords;
  // Keywords such as for, if, etc that take as input an in-parentheses block
  // and an
  // in-curly-brace block.
  HashedList<std::string, HashFunctions::hashFunction> controlKeyWords;
  MapList<
    std::string,
    CodeFormatter::Element::Type,
    HashFunctions::hashFunction
  > elementTypes;
  // A list of elements interpreted as operators.
  HashedList<std::string, HashFunctions::hashFunction> operatorList;
  // A list of operator overrides.
  // An operator K given in the key overrides any operator V given in the
  // values.
  // In other words, the expression
  // xVyKz is to be interpreted as xV(yKz) rather than as (xVy)Kz
  MapList<
    std::string,
    HashedList<std::string, HashFunctions::hashFunction>,
    HashFunctions::hashFunction
  > operatorOverrides;
  int maximumDesiredLineLength;
  int indexCurrentlyConsumed;
  const int tabLength = 2;
  const int dummyElements = 6;
  // A class to represent
  CodeFormatter();
  static bool formatCPPDirectory(
    const std::string& inputDirectory,
    bool inPlace,
    std::stringstream* comments
  );
  static bool formatCalculatorInPlace();
  bool formatCPPSourceCode(
    const std::string& inputFileName,
    const std::string& output,
    const std::string& outputOnFail,
    std::stringstream* comments,
    bool logDebugInfo
  );
private:
  void addOperatorOverride(
    const std::string& overridingOperator,
    const List<std::string>& overridden
  );
  bool shouldSeparateWithSpace(
    const std::string& left, const std::string& right
  );
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
    const std::string& output,
    const std::string& outputOnFail,
    std::stringstream* comments
  );
  bool parsingSucceeded() const;
  std::string toStringLinks();
  static bool isIdentifierWord(const std::string& input);
  bool isLetterLike(char input);
  void wirePointersRecursively(
    CodeFormatter::Element& current,
    CodeFormatter::Element* parent,
    int indexInParent
  );
  bool isOperatorSuitableForNormalization(
    const CodeFormatter::Element& element
  );
  void normalizeBinaryOperationsRecursively(CodeFormatter::Element& current);
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
  bool mustSplitWithWhitespace(
    const CodeFormatter::Element& leftAtom,
    const CodeFormatter::Element& rightAtom
  );
  void applyNewLineExceptions(
    CodeFormatter::Element& leftAtom, CodeFormatter::Element& rightAtom
  );
};

#endif // header_source_code_formatter_ALREADY_INCLUDED
