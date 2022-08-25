// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_calculator_html_interpretation_ALREADY_INCLUDED
#define header_calculator_html_interpretation_ALREADY_INCLUDED

#include "general_lists.h"
#include "database.h"
#include "calculator_problem_storage.h"
#include <ctime>

class CalculatorHTML;
class TopicElement;

class TopicElementParser {
public:
  class TopicLine {
  public:
    friend std::ostream& operator<<(
      std::ostream& output, const TopicLine& line
    ) {
      output << line.toString();
      return output;
    }
    int topicType;
    std::string tag;
    std::string contentTrimmedWhiteSpace;
    void makeError(const std::string& message);
    void MakeEmpty();
    TopicElement toTopicElement() const;
    std::string toString() const;
    bool accountIfStateChanger(CalculatorHTML& owner) const;
  };

  CalculatorHTML* owner;
  MapList<
    std::string,
    TopicElement,
    HashFunctions::hashFunction<std::string>
  > topics;
  MapList<
    std::string,
    List<TopicElementParser::TopicLine>,
    HashFunctions::hashFunction<std::string>
  > knownTopicBundles;
  HashedList<std::string> loadedTopicBundleFiles;
  MapList<
    std::string, int, HashFunctions::hashFunction<std::string>
  > elementTypes;
  MapList<int, std::string, HashFunctions::hashFunction> elementNames;
  List<TopicElementParser::TopicLine> crawled;
  List<TopicElementParser::TopicLine> bundleStack;
  List<TopicElement> elements;
  int maximumTopics;
  TopicElementParser();
  void parseTopicList(const std::string& inputString);
  void compressTopicLines();
  void computeIds();
  void computeTopicHierarchy();
  void computeTopicNumbers();
  void computeTopicHierarchyPartOne();
  void computeTopicHierarchyPartTwo();
  void addNewTopicElementFromLine(const TopicLine& input);
  void compressOneTopicLine(const TopicLine& input, CalculatorHTML& owner);
  void crawl(const std::string& inputString);
  void exhaustCrawlStack();
  void loadTopicBundleFile(TopicLine& input);
  void insertTopicBundle(TopicLine& input);
  bool checkInitialization();
  bool checkConsistencyParsed();
  bool checkNoErrors(std::stringstream* commentsOnFailure);
  bool checkProblemsOpen(std::stringstream* commentsOnFailure);
  bool checkTopicPdfs(std::stringstream* commentsOnFailure);
  void addTopic(TopicElement& inputElt, int index);
  std::string toString() const;
  void initializeElementTypes();
  TopicLine extractLine(const std::string& inputNonTrimmed);
  class Test {
  public:
    std::stringstream comments;
    static bool all();
    static bool defaultTopicListsOKCrashOnFailure();
    static bool defaultPdfsOKCrashOnFailure();
    bool defaultPdfsOK(int& whichTopic);
    bool defaultTopicListsOK();
  };
};

class TopicElement {
public:
  static int scoreButtonCounter;
  int type;
  class types {
  public:
    static const int unknown = 0;
    static const int empty = 1;
    // Note: the fact that:
    // chapter < section < topic < problem
    // is used when determining parent-child
    // relations between topics.
    static const int chapter = 2;
    static const int section = 3;
    static const int topic = 4;
    static const int problem = 5;
    static const int error = 6;
    static const int texHeader = 7;
    // Service elements.
    static const int title = 8;
    static const int video = 9;
    static const int videoHandwritten = 10;
    static const int slidesLatex = 11;
    static const int slidesSource = 12;
    static const int homeworkLatex = 13;
    static const int homeworkSource = 14;
    static const int homeworkSolutionSource = 15;
    static const int handwrittenSolutions = 16;
    static const int slidesSourceHeader = 17;
    static const int homeworkSourceHeader = 18;
    static const int loadTopicBundles = 19;
    static const int topicBundle = 20;
    static const int bundleBegin = 21;
    static const int bundleEnd = 22;
  };

  bool flagContainsProblemsNotInSubsection;
  bool flagSubproblemHasNoWeight;
  bool flagHasLectureTag;
  std::string problemNumberString;
  List<int> problemNumber;
  List<int> parentTopics;
  List<int> immediateChildren;
  int indexInParent;
  int totalSubSectionsUnderME;
  int totalSubSectionsUnderMeIncludingEmptySubsections;
  std::string id;
  // <- For problems the id is the problem file name. For all other topic
  // elements the id is the title of the element.
  List<std::string> idsDeadlines;
  List<std::string> deadlinesPerSection;
  List<std::string> deadlinesPerSectionFormatted;
  List<bool> deadlinesAreInherited;
  std::string idBase64;
  std::string idDeadlineReport;
  std::string idDeadlineTable;
  std::string idDeadlineButton;
  std::string title;
  std::string video;
  std::string videoHandwritten;
  std::string querySlides;
  std::string queryHomework;
  std::string handwrittenSolution;
  List<std::string> sourceSlides;
  List<std::string> sourceHomework;
  List<bool> sourceHomeworkIsSolution;
  std::string problemFileName;
  std::string error;
  std::string displayTitle;
  std::string displayVideoLink;
  std::string displayVideoHandwrittenLink;
  std::string displayHandwrittenSolution;
  std::string displayProblemLink;
  std::string displayResourcesLinks;
  std::string displayDeadlinE;
  std::string displayScore;
  std::string displayModifyWeight;
  std::string displayModifyDeadline;
  Rational totalPointsEarned;
  Rational pointsEarnedInProblemsThatAreImmediateChildren;
  Rational maxPointsInAllChildren;
  //  Rational numAnsweredInAllChildren;
  //  Rational maxCorrectAnswersInAllChildren;
  void computeID(int elementIndex, TopicElementParser& owner);
  void reset();
  friend std::ostream& operator<<(
    std::ostream& output, const TopicElement& element
  ) {
    output << element.toString();
    return output;
  }
  bool isError();
  bool problemOpensIfAvailable(std::stringstream* commentsOnFailure);
  bool pdfsOpenIfAvailable(
    CalculatorHTML& owner, std::stringstream* commentsOnFailure
  );
  bool pdfSlidesOpenIfAvailable(
    CalculatorHTML& owner, std::stringstream* commentsOnFailure
  );
  bool pdfHomeworkOpensIfAvailable(
    CalculatorHTML& owner, std::stringstream* commentsOnFailure
  );
  std::string toString() const;
  JSData toJSON(CalculatorHTML& owner);
  void computeSlides(CalculatorHTML& owner);
  JSData computeSlidesJSON(CalculatorHTML& owner);
  void computeHomework(CalculatorHTML& owner);
  JSData computeHomeworkJSON(CalculatorHTML& owner);
  void computeLinks(CalculatorHTML& owner, bool plainStyle);
  TopicElement();
  void makeError(const std::string& message);
  bool mergeTopicLine(const TopicElementParser::TopicLine& input);
};

class ProblemResources {
public:
  std::string video;
  std::string slides;
  std::string slidesPrintable;
  std::string deadline;
  Rational pointsScored;
  Rational maxPoints;
};

class CalculatorHTML {
private:
  bool interpretHtmlOneAttemptPartTwo(
    Calculator& interpreter,
    std::stringstream& comments,
    double startTime,
    std::stringstream& outBody,
    std::stringstream& outHeadPt2
  );
public:
  int numberOfInterpretationAttempts;
  int maxInterpretationAttempts;
  int numberOfProblemsFound;
  int numberOfVideosFound;
  int numberOfVideosWithSlidesFound;
  int numberOfVideosHandwrittenFound;
  int numberOfSlidesFound;
  int numberOfHandwrittenSolutionsFound;
  int numberOfAnswerIdsMathquilled;
  List<uint32_t> randomSeedPerAttempt;
  bool flagParentInvestigated;
  bool flagIsForReal;
  bool flagLoadedFromDB;
  bool flagDoPrependEditPagePanel;
  bool flagSectionsPrepared;
  bool flagIncludeStudentScores;
  bool flagTopicTableStarted;
  bool flagTopicSectionStarted;
  bool flagTopicSubSectionStarted;
  bool flagTopicChapterStarted;
  double timeToParseHtml;
  List<SyntacticElementHTML> content;
  List<double> timePerAttempt;
  List<List<double> > timeIntermediatePerAttempt;
  List<List<std::string> > timeIntermediateComments;
  UserCalculator currentUser;
  static std::string stringScoredQuizzes;
  static std::string stringPracticE;
  static std::string stringProblemLink;
  List<std::string> slidesSourcesHeaders;
  List<std::string> sourcesHomeworkHeaders;
  std::string fileName;
  std::string relativePhysicalFileNameWithFolder;
  struct answerLabels {
    static std::string properties;
    static std::string idPanel;
    static std::string answerHighlight;
    static std::string idEquationEditorElement;
    static std::string idButtonContainer;
    static std::string mathQuillPanelOptions;
    static std::string idPureLatex;
    static std::string idButtonSubmit;
    static std::string idButtonInterpret;
    static std::string idButtonAnswer;
    static std::string idButtonSolution;
    static std::string idVerificationSpan;
    static std::string previousAnswers;
  };

  std::string outputHtmlBodyNoTag;
  std::string outputHtmlHeadNoTag;
  std::string outputProblemNavigatioN;
  std::string outputDeadlineString;
  std::string outputProblemLabel;
  std::string outputProblemTitle;
  std::string outputDebugInformationBody;
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > outputScripts;
  std::stringstream logCommandsProblemGeneratioN;
  std::string courseHome;
  std::string topicListJavascriptWithTag;
  static const std::string bugsGenericMessage;
  HashedList<std::string> tagKeysNoValue;
  List<std::string> calculatorTopicElementNames;
  List<std::string> autoCompleteExtras;
  //  List<std::string> answerFirstCorrectSubmission;
  Selection studentTagsAnswered;
  ProblemData problemData;
  List<std::string> answerHighlights;
  int topicLectureCounter;
  std::string topicListContent;
  std::string topicListFileName;
  HashedList<std::string> problemNamesNoTopics;
  TopicElementParser topics;
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > scripts;
  List<std::string> databaseStudentSections;
  bool flagLoadedSuccessfully;
  bool flagLoadedClassDataSuccessfully;
  class Parser {
  public:
    enum Phase {
      none,
      leftAngleBracketEncountered,
      startedOpenTag,
      startedCloseTag,
      startedOpenTagGotBackslash,
      propertyEncountered,
      equalityEncountered,
      singleQuoteOpened,
      doubleQuoteOpened,
      closeTagWaitingForRightAngleBracket,
      error,
    };
    Phase phase;
    std::string inputHtml;
    CalculatorHTML* owner;
    static HashedList<std::string, HashFunctions::hashFunction>
    calculatorClasses;
    static HashedList<std::string, HashFunctions::hashFunction>
    calculatorClassesAnswerFields;
    List<char> splittingChars;
    List<SyntacticElementHTML> elementStack;
    Parser();
    void reset();
    bool parseHTML(std::stringstream* comments);
    bool reduceMore();
    void initBuiltInSpanClasses();
    std::string toStringParsingStack(List<SyntacticElementHTML>& stack);
    bool isWhiteSpace(const std::string& input);
    bool isSplittingChar(const std::string& input);
    bool isCalculatorTag(const SyntacticElementHTML& input);
    bool consumeContentStandard();
    bool consumeAfterAnswerHighlight();
    bool consumeAfterLeftAngleBracket();
    bool consumeTagOpened();
    bool closeOpenTag(int tagOffsetNegative);
    bool consumeTagOpenedGotBackSlash();
    bool consumeCloseTagOpened();
    bool consumeCloseTagWaitingForRightAngleBracket();
    bool consumeAfterProperty();
    bool consumeAfterEquality();
    bool consumeAfterQuote(const std::string& quoteSymbol);
    bool consumeBackSlashInQuote(const std::string& quoteSymbol);
    bool consumeErrorOrMergeInCalculatorTag(
      int calculatorTagNegativeOffset, const std::string& errorMessage
    );
    bool consumeErrorOrMergeInCalculatorTagRetainLast(
      int calculatorTagNegativeOffset, const std::string& errorMessage
    );
    bool reduceStackMergeContents(int numberOfElementsToRemove);
    bool reduceStackMergeContentsRetainLast(int numberOfElementsToRemove);
    bool setLastToError(const std::string& errorMessage);
    std::string toStringPhaseInfo();
    bool canBeMerged(
      const SyntacticElementHTML& left, const SyntacticElementHTML& right
    );
    bool setTagClassFromOpenTag(SyntacticElementHTML& output);
    bool isStateModifierApplyIfYes(SyntacticElementHTML& inputElt);
  };

  Parser parser;
  // ///////////////
  int getAnswerIndex(const std::string& desiredAnswerId);
  bool parseHTML(std::stringstream* comments) {
    return this->parser.parseHTML(comments);
  }
  bool checkContent(std::stringstream* comments);
  bool loadMe(
    bool doLoadDatabase,
    const std::string& inputRandomSeed,
    std::stringstream* commentsOnFailure
  );
  bool loadAndParseTopicIndex(int index, std::stringstream& comments);
  bool loadAndParseTopicList(std::stringstream& comments);
  bool loadDatabaseInfo(std::stringstream& comments);
  std::string cleanUpFileName(const std::string& inputLink);
  void initAutocompleteExtras();
  void initBuiltInSpanClasses();
  void loadFileNames();
  bool extractContent(std::stringstream* comments);
  bool processParsedElements(std::stringstream* comments);
  class SolutionProcessor {
  public:
    List<SyntacticElementHTML> contentWithoutSolutionElements;
    List<SyntacticElementHTML> currentSolution;
    Answer* currentAnswer;
    SolutionProcessor();
  };

  bool processSolutions(std::stringstream* comments);
  bool processOneSolution(
    SyntacticElementHTML& input,
    SolutionProcessor& processor,
    std::stringstream* comments
  );
  bool extractAnswerIdsOnce(
    SyntacticElementHTML& element, std::stringstream* comments
  );
  bool extractAnswerIds(std::stringstream* comments);
  bool extractOneAnswerId(
    SyntacticElementHTML& input, std::stringstream* comments
  );
  bool interpretHtml(std::stringstream* comments);
  bool interpretHtmlOneAttempt(
    Calculator& interpreter, std::stringstream& comments
  );
  void computeProblemLabel();
  void computeBodyDebugString();
  std::string toStringInterprettedCommands(
    Calculator& calculator, List<SyntacticElementHTML>& elements
  );
  void logProblemGenerationObsolete(Calculator& interpreter);
  bool processExecutedCommands(
    Calculator& interpreter,
    List<SyntacticElementHTML>& elements,
    std::stringstream& comments
  );
  bool processOneExecutedCommand(
    Calculator& interpreter,
    SyntacticElementHTML& element,
    std::stringstream& comments
  );
  bool prepareAnswerElements(std::stringstream& comments);
  bool interpretAnswerHighlights(std::stringstream& comments);
  bool interpretAnswerElements(std::stringstream& comments);
  bool interpretOneAnswerElement(SyntacticElementHTML& inputOutput);
  bool prepareAndExecuteCommands(
    Calculator& interpreter, std::stringstream* comments
  );
  std::string prepareUserInputBoxes();
  bool prepareCommandsAnswerOnGiveUp(
    Answer& answer, std::stringstream* comments
  );
  bool prepareCommentsBeforeSubmission(
    Answer& answer, std::stringstream* comments
  );
  bool prepareCommentsBeforeInterpretation(
    Answer& answer, std::stringstream* comments
  );
  bool exrtactSolutionCommands(
    Answer& answer, std::stringstream* comments
  );
  bool prepareCommandsAnswer(Answer& answer, std::stringstream* comments);
  bool prepareCommandsGenerateProblem(std::stringstream* comments);
  std::string getProblemHeaderEnclosure();
  std::string getProblemHeaderWithoutEnclosure();
  bool storeRandomSeedCurrent(std::stringstream* commentsOnFailure);
  bool parseHTMLPrepareCommands(std::stringstream* comments);
  bool prepareCommands(std::stringstream* comments);
  void interpretNotByCalculatorNotAnswer(SyntacticElementHTML& inputOutput);
  void interpretIfAnswer(SyntacticElementHTML& inputOutput);
  std::string getDeadlineNoInheritance(const std::string& id);
  std::string getDeadline(
    const std::string& problemName,
    const std::string& sectionNumber,
    bool& outputIsInherited
  );
  bool mergeOneProblemAdminData(
    const std::string& inputProblemName,
    ProblemData& inputProblemInfo,
    std::stringstream& commentsOnFailure
  );
  bool mergeProblemWeightAndStore(
    std::string& incomingProblemInfo,
    std::stringstream* commentsOnFailure
  );
  bool mergeProblemDeadlineAndStore(
    std::string& incomingProblemInfo,
    std::stringstream* commentsOnFailure
  );
  bool mergeProblemWeight(
    const JSData& inputJSON,
    MapList<
      std::string,
      ProblemData,
      HashFunctions::hashFunction<std::string>
    >& outputAppendProblemInfo,
    bool checkFileExistence,
    std::stringstream* commentsOnFailure
  );
  bool mergeProblemDeadline(
    const JSData& inputJSON,
    MapList<
      std::string,
      ProblemData,
      HashFunctions::hashFunction<std::string>
    >& outputAppendProblemInfo,
    std::stringstream* commentsOnFailure
  );
  bool storeProblemWeights(
    MapList<
      std::string,
      ProblemData,
      HashFunctions::hashFunction<std::string>
    >& toStore,
    std::stringstream* commentsOnFailure
  );
  bool storeProblemDeadlines(
    MapList<
      std::string,
      ProblemData,
      HashFunctions::hashFunction<std::string>
    >& toStore,
    std::stringstream* commentsOnFailure
  );
  QuerySet toQuerySetProblemWeights(
    MapList<
      std::string,
      ProblemData,
      HashFunctions::hashFunction<std::string>
    >& inputProblemInfo
  );
  JSData toJSONDeadlines(
    MapList<
      std::string,
      ProblemData,
      HashFunctions::hashFunction<std::string>
    >& inputProblemInfo
  );
  std::string toStringDeadline(
    const std::string& topicID,
    bool problemAlreadySolved,
    bool returnEmptyStringIfNoDeadline,
    bool isSection
  );
  void computeDeadlinesAllSections(TopicElement& inputOutput);
  void computeDeadlinesAllSectionsNoInheritance(TopicElement& inputOutput);
  JSData toStringTopicListJSON(std::stringstream* comments);
  std::string toStringProblemInfo(
    const std::string& fileName, const std::string& stringToDisplay = ""
  );
  static std::string toStringLinkFromProblem(
    const std::string& fileName,
    bool practiceMode = true,
    int randomSeed = - 1
  );
  std::string toStringLinkFromFileName(const std::string& fileName);
  std::string toStringLinkCurrentAdmin(
    const std::string& displayString,
    bool setDebugFlag,
    bool includeRandomSeed
  );
  std::string toStringCalculatorProblemSourceFromFileName(
    const std::string& fileName
  );
  void interpretGenerateLink(SyntacticElementHTML& inputOutput);
  std::string toStringProblemScoreFull(const std::string& fileName);
  std::string toStringProblemScoreShort(
    const std::string& fileName, bool& outputAlreadySolved
  );
  bool computeAnswerRelatedStrings(SyntacticElementHTML& inputOutput);
  void interpretGenerateStudentAnswerButton(SyntacticElementHTML& inputOutput);
  bool prepareSectionList(std::stringstream& commentsOnFailure);
  void interpretManageClass(SyntacticElementHTML& inputOutput);
  bool computeTopicListAndPointsEarned(std::stringstream& commentsOnFailure);
  void interpretAccountInformationLinks(SyntacticElementHTML& inputOutput);
  void interpretJavascripts(SyntacticElementHTML& inputOutput);
  JSData getEditorBoxesHTML();
  void loadCurrentProblemItem(
    bool needToLoadDatabaseMayIgnore,
    const std::string& inputRandomSeed,
    std::stringstream* commentsOnFailure
  );
  void figureOutCurrentProblemList(std::stringstream& comments);
  std::string loadAndInterpretCurrentProblemItemJSON(
    bool needToLoadDatabaseMayIgnore,
    const std::string& desiredRandomSeed,
    std::stringstream* commentsOnFailure
  );
  static unsigned int hashFunction(const CalculatorHTML& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return HashFunctions::hashFunction<std::string>(this->fileName);
  }
  bool operator==(const CalculatorHTML& other) const {
    return this->fileName == other.fileName;
  }
  std::string toStringAllSectionDeadlines(
    const std::string& topicID, bool isSection
  );
  std::string toStringOneDeadlineFormatted(
    const std::string& topicID,
    const std::string& sectionNumber,
    bool problemAlreadySolved,
    bool returnEmptyStringIfNoDeadline,
    bool isSection
  );
  std::string toStringCalculatorArgumentsForProblem(
    const std::string& requestType,
    const std::string& studentView,
    const std::string& studentSection = "",
    bool includeRandomSeedIfAppropriate = false
  ) const;
  std::string toStringExtractedCommands();
  std::string toStringContent();
  std::string toStringParsedElements() const;
  CalculatorHTML();
  class Test {
  public:
    class OneProblemTest {
    public:
      class OneAnswer {
      public:
        std::string answerId;
        std::string answerIdWebAPI;
        std::string builtInAnswer;
        std::string builtInAnswerEncoded;
        JSData builtInAnswerAPICall;
        JSData builtInAnswerReply;
        bool flagBuiltInGenerated;
        bool flagBuiltInWorks;
        OneAnswer();
      };

      std::string fileName;
      std::string errorLoad;
      std::string errorInterpretation;
      std::string errorAnswers;
      bool flagInterpretationSuccess;
      bool flagAllBuiltInAnswersOK;
      bool flagSuccess;
      int randomSeed;
      List<OneAnswer> answers;
      OneProblemTest();
      bool run();
      std::string toStringHTMLTableRow(int rowIndex);
    };

    int filesToInterpret;
    int firstFileIndex;
    int randomSeed;
    bool flagCorrectedTotalFiles;
    List<std::string> fileNamesAll;
    List<std::string> fileExtensionsAll;
    List<std::string> fileNames;
    List<OneProblemTest> results;
    std::string errorComments;
    Test();
    bool computeTotalFiles();
    static bool builtInMultiple(
      int inputFirstFileIndex,
      int inputFilesToInterpret,
      int inputRandomSeed,
      int numberOfRepetitions,
      std::stringstream* comments
    );
    bool builtIn(
      int inputFirstFileIndex, int inputFilesToInterpret, int inputRandomSeed
    );
    static bool builtInCrashOnFailure();
    static bool all();
    static bool parsingTest();
    std::string toStringSummary();
    std::string toHTMLBuiltIn();
    std::string toHTMLDebug();
  };
};

class Course {
public:
  std::string courseTemplate;
  std::string courseTopicsNoFolder;
  std::string title;
  std::string flagRoughDraft;
  bool isEmpty();
  void reset();
  std::string courseTopicsWithFolder();
  std::string toString() const;
  JSData toJSON() const;
  class Test{
  public:
    static bool all();
    static bool setDeadlines();
  };
};

class CourseList {
public:
  List<Course> allCourses;
  std::string errorMessage;
  bool loadFromString(const std::string& input);
  std::string toHtml();
  bool load();
  JSData toJSON();
};

#endif // header_calculator_html_interpretation_ALREADY_INCLUDED
