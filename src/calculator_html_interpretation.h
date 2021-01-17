// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderHtmlInterpretationAlreadyDefined
#define vpfHeaderHtmlInterpretationAlreadyDefined
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
    friend std::ostream& operator<<(std::ostream& output, const TopicLine& line) {
      output << line.toString();
      return output;
    }
    int theType;
    std::string tag;
    std::string contentTrimmedWhiteSpace;
    void makeError(const std::string& message);
    void MakeEmpty();
    TopicElement toTopicElement() const;
    std::string toString() const;
    bool accountIfStateChanger(CalculatorHTML& owner) const;
  };
  CalculatorHTML* owner;
  MapList<std::string, TopicElement, MathRoutines::hashString> theTopics;
  MapList<std::string, List<TopicElementParser::TopicLine>, MathRoutines::hashString> knownTopicBundles;
  HashedList<std::string, MathRoutines::hashString> loadedTopicBundleFiles;

  MapList<std::string, int, MathRoutines::hashString> elementTypes;
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
  void addNewTopicElementFromLine(const TopicLine &input);
  void compressOneTopicLine(const TopicLine &input, CalculatorHTML& owner);
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
    bool defaultPdfsOK();
    bool defaultTopicListsOK();
  };
};

class TopicElement {
public:
  static int scoreButtonCounter;
  int type;
  class types {
  public:
    static const int unknown                = 0 ;
    static const int empty                  = 1 ;
    // Note: the fact that:
    // chapter < section < topic < problem
    // is used when determining parent-child
    // relations between topics.
    static const int chapter                = 2 ;
    static const int section                = 3 ;
    static const int topic                  = 4 ;
    static const int problem                = 5 ;
    static const int error                  = 6 ;
    static const int texHeader              = 7 ;
    // service elements
    static const int title                  = 8 ;
    static const int video                  = 9 ;
    static const int videoHandwritten       = 10;
    static const int slidesLatex            = 11;
    static const int slidesSource           = 12;
    static const int homeworkLatex          = 13;
    static const int homeworkSource         = 14;
    static const int homeworkSolutionSource = 15;
    static const int handwrittenSolutions   = 16;
    static const int slidesSourceHeader     = 17;
    static const int homeworkSourceHeader   = 18;
    static const int loadTopicBundles       = 19;
    static const int topicBundle            = 20;
    static const int bundleBegin            = 21;
    static const int bundleEnd              = 22;
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
  std::string id; //<- for problems the id is the problem file name. For all other topic
  // elements the id is the title of the element.
  List<std::string> idsDeadlines;
  List<std::string> deadlinesPerSectioN;
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
  friend std::ostream& operator << (std::ostream& output, const TopicElement& theElt) {
    output << theElt.toString();
    return output;
  }
  bool isError();
  bool problemOpensIfAvailable(std::stringstream* commentsOnFailure);
  bool pdfsOpenIfAvailable(CalculatorHTML& owner, std::stringstream* commentsOnFailure);
  bool pdfSlidesOpenIfAvailable(CalculatorHTML& owner, std::stringstream* commentsOnFailure);
  bool pdfHomeworkOpensIfAvailable(CalculatorHTML& owner, std::stringstream* commentsOnFailure);
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
  uint32_t randomSeedCurrent;
  List<uint32_t> randomSeedPerAttempt;
  bool flagParentInvestigated;
  bool flagIsForReal;
  bool flagLoadedFromDB;
  bool flagTagHtmlPresent;
  bool flagTagHeadPresent;
  bool flagTagBodyPresent;
  bool flagDoPrependEditPagePanel;
  bool flagMathQuillWithMatrices;
  bool flagSectionsPrepared;
  bool flagIncludeStudentScores;
  bool flagTopicTableStarted;
  bool flagTopicSectionStarted;
  bool flagTopicSubSectionStarted;
  bool flagTopicChapterStarted;
  double timeToParseHtml;
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
  std::string inputHtml;
  struct answerLabels {
    static std::string properties;
    static std::string idPanel;
    static std::string answerHighlight;
    static std::string idEquationEditorElement;
    static std::string idEquationEditorElementLocation;
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
  MapList<std::string, std::string, MathRoutines::hashString> outputScripts;

  std::stringstream logCommandsProblemGeneratioN;
  std::string courseHome;
  std::string topicListJavascriptWithTag;

  static const std::string bugsGenericMessage;
  HashedList<std::string, MathRoutines::hashString> tagKeysNoValue;
  List<std::string> calculatorTopicElementNames;
  List<std::string> calculatorTagsRecordedLiterally;
  List<std::string> calculatorClasses;
  List<std::string> calculatorClassesAnswerFields;
  List<std::string> autoCompleteExtras;
  List<char> splittingChars;
  List<SyntacticElementHTML> elementStack;
  List<SyntacticElementHTML> content;
  //  List<std::string> answerFirstCorrectSubmission;
  Selection studentTagsAnswered;
  ProblemData theProblemData;
  List<std::string> answerHighlights;
  int topicLectureCounter;
  std::string topicListContent;
  std::string topicListFileName;
  HashedList<std::string, MathRoutines::hashString> problemNamesNoTopics;
  TopicElementParser topics;
  MapList<std::string, std::string, MathRoutines::hashString> theScripts;
  List<std::string> databaseStudentSections;
  bool flagLoadedSuccessfully;
  bool flagLoadedClassDataSuccessfully;
  /////////////////
  int getAnswerIndex(const std::string& desiredAnswerId);
  bool checkContent(std::stringstream* comments);
  bool canBeMerged(const SyntacticElementHTML& left, const SyntacticElementHTML& right);
  bool loadMe(bool doLoadDatabase, const std::string& inputRandomSeed, std::stringstream* commentsOnFailure);
  bool loadAndParseTopicList(std::stringstream& comments);
  bool loadDatabaseInfo(std::stringstream& comments);
  std::string cleanUpFileName(const std::string& inputLink);
  void initAutocompleteExtras();
  void initBuiltInSpanClasses();
  bool parseHTML(std::stringstream* comments);
  bool parseHTMLPrepareCommands(std::stringstream* comments);
  bool isSplittingChar(const std::string& input);
  void loadFileNames();
  bool isStateModifierApplyIfYes(SyntacticElementHTML& inputElt);
  bool extractAnswerIds(std::stringstream* comments);
  bool interpretHtml(std::stringstream* comments);
  bool interpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments);
  void computeProblemLabel();
  void computeBodyDebugString();
  std::string toStringInterprettedCommands(Calculator& theInterpreter, List<SyntacticElementHTML>& theElements);
  void logProblemGenerationObsolete(Calculator& theInterpreter);
  bool interpretProcessExecutedCommands(
    Calculator& theInterpreter, List<SyntacticElementHTML>& theElements, std::stringstream& comments
  );
  bool prepareAnswerElements(std::stringstream& comments);
  bool interpretAnswerHighlights(std::stringstream& comments);
  bool interpretAnswerElements(std::stringstream& comments);
  bool interpretOneAnswerElement(SyntacticElementHTML& inputOutput);
  bool prepareAndExecuteCommands(Calculator& theInterpreter, std::stringstream* comments);
  std::string prepareUserInputBoxes();
  bool prepareCommandsAnswerOnGiveUp(Answer& theAnswer, std::stringstream* comments);
  bool prepareCommentsBeforeSubmission(Answer& theAnswer, std::stringstream* comments);
  bool prepareCommentsBeforeInterpretation(Answer& theAnswer, std::stringstream* comments);
  bool prepareCommandsSolution(Answer& theAnswer, std::stringstream* comments);
  bool prepareCommandsAnswer(Answer& theAnswer, std::stringstream* comments);
  bool prepareCommandsGenerateProblem(std::stringstream* comments);
  std::string getProblemHeaderEnclosure();
  std::string getProblemHeaderWithoutEnclosure();
  bool setTagClassFromOpenTag(SyntacticElementHTML& output);
  bool setTagClassFromCloseTag(SyntacticElementHTML& output);
  bool storeRandomSeedCurrent(std::stringstream* commentsOnFailure);
  bool prepareCommands(std::stringstream* comments);
  std::string cleanUpCommandString(const std::string& inputCommand);
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
    std::string& incomingProblemInfo, std::stringstream* commentsOnFailure
  );
  bool mergeProblemDeadlineAndStore(
    std::string& incomingProblemInfo, std::stringstream* commentsOnFailure
  );
  bool mergeProblemWeight(
    const JSData& inputJSON,
    MapList<std::string, ProblemData, MathRoutines::hashString>& outputAppendProblemInfo,
    bool checkFileExistence,
    std::stringstream* commentsOnFailure
  );
  bool mergeProblemDeadline(
    const JSData& inputJSON,
    MapList<std::string, ProblemData, MathRoutines::hashString>& outputAppendProblemInfo,
    std::stringstream* commentsOnFailure
  );
  bool storeProblemWeights(
    MapList<std::string, ProblemData, MathRoutines::hashString>& toStore,
    std::stringstream* commentsOnFailure
  );
  bool storeProblemDeadlines(
    MapList<std::string, ProblemData, MathRoutines::hashString>& toStore,
    std::stringstream* commentsOnFailure
  );
  QuerySet toQuerySetProblemWeights(MapList<std::string, ProblemData, MathRoutines::hashString>& inputProblemInfo);
  JSData toJSONDeadlines(MapList<std::string, ProblemData, MathRoutines::hashString>& inputProblemInfo);
  std::string toStringDeadline(
    const std::string& topicID, bool problemAlreadySolved, bool returnEmptyStringIfNoDeadline, bool isSection
  );
  void computeDeadlinesAllSections(TopicElement& inputOutput);
  void computeDeadlinesAllSectionsNoInheritance(TopicElement& inputOutput);
  JSData toStringTopicListJSON(std::stringstream* comments);
  std::string toStringProblemInfo(const std::string& theFileName, const std::string& stringToDisplay = "");
  static std::string toStringLinkFromProblem(const std::string& theFileName, bool practiceMode = true, int randomSeed = - 1);
  std::string toStringLinkFromFileName(const std::string& theFileName);
  std::string toStringLinkCurrentAdmin(const std::string& displayString, bool setDebugFlag, bool includeRandomSeed);
  std::string toStringCalculatorProblemSourceFromFileName(const std::string& theFileName);
  void interpretGenerateLink(SyntacticElementHTML& inputOutput);
  std::string toStringProblemScoreFull(const std::string& theFileName);
  std::string toStringProblemScoreShort(const std::string& theFileName, bool& outputAlreadySolved);
  bool computeAnswerRelatedStrings(SyntacticElementHTML& inputOutput);
  void interpretGenerateStudentAnswerButton(SyntacticElementHTML& inputOutput);
  bool prepareSectionList(std::stringstream& commentsOnFailure);
  void interpretManageClass(SyntacticElementHTML& inputOutput);
  void interpretTopicList(SyntacticElementHTML& inputOutput);
  bool computeTopicListAndPointsEarned(std::stringstream& commentsOnFailure);
  void interpretTableOfContents(SyntacticElementHTML& inputOutput);
  void interpretAccountInformationLinks(SyntacticElementHTML& inputOutput);
  void interpretJavascripts(SyntacticElementHTML& inputOutput);
  JSData getJavascriptMathQuillBoxesForJSON();
  void loadCurrentProblemItem(
    bool needToLoadDatabaseMayIgnore, const std::string& inputRandomSeed, std::stringstream* commentsOnFailure
  );
  void figureOutCurrentProblemList(std::stringstream& comments);
  std::string LoadAndInterpretCurrentProblemItemJSON(
    bool needToLoadDatabaseMayIgnore, const std::string& desiredRandomSeed, std::stringstream* commentsOnFailure
  );
  static unsigned int hashFunction(const CalculatorHTML& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return MathRoutines::hashString(this->fileName);
  }
  bool operator==(const CalculatorHTML& other) const {
    return this->fileName == other.fileName;
  }
  std::string toStringAllSectionDeadlines(const std::string& topicID, bool isSection);
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
  std::string ToStringExtractedCommands();
  std::string toStringContent();
  std::string toStringParsingStack(List<SyntacticElementHTML>& theStack);
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
      int inputFirstFileIndex,
      int inputFilesToInterpret,
      int inputRandomSeed
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
};

class CourseList {
public:
  List<Course> theCourses;
  std::string errorMessage;
  bool loadFromString(const std::string& input);
  std::string toHtml();
  bool load();
  JSData toJSON();
};

#endif
