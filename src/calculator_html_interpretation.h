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
    TopicElement ToTopicElement() const;
    std::string toString() const;
    bool AccountIfStateChanger(CalculatorHTML& owner) const;
  };
  CalculatorHTML* owner;
  MapList<std::string, TopicElement, MathRoutines::HashString> theTopics;
  MapList<std::string, List<TopicElementParser::TopicLine>, MathRoutines::HashString> knownTopicBundles;
  HashedList<std::string, MathRoutines::HashString> loadedTopicBundleFiles;

  MapList<std::string, int, MathRoutines::HashString> elementTypes;
  MapList<int, std::string, MathRoutines::IntUnsignIdentity> elementNames;
  List<TopicElementParser::TopicLine> crawled;
  List<TopicElementParser::TopicLine> bundleStack;
  List<TopicElement> elements;
  int maximumTopics;
  TopicElementParser();
  void ParseTopicList(const std::string& inputString);
  void CompressTopicLines();
  void ComputeIds();
  void ComputeTopicHierarchy();
  void ComputeTopicNumbers();
  void ComputeTopicHierarchyPartOne();
  void ComputeTopicHierarchyPartTwo();
  void AddNewTopicElementFromLine(const TopicLine &input);
  void CompressOneTopicLine(const TopicLine &input, CalculatorHTML& owner);
  void Crawl(const std::string& inputString);
  void ExhaustCrawlStack();
  void LoadTopicBundleFile(TopicLine& input);
  void InsertTopicBundle(TopicLine& input);
  bool checkInitialization();
  bool CheckConsistencyParsed();
  bool CheckNoErrors(std::stringstream* commentsOnFailure);
  bool CheckProblemsOpen(std::stringstream* commentsOnFailure);
  bool CheckTopicPdfs(std::stringstream* commentsOnFailure);
  void AddTopic(TopicElement& inputElt, int index);
  std::string toString() const;
  void initializeElementTypes();
  TopicLine ExtractLine(const std::string& inputNonTrimmed);
  class Test {
  public:
    std::stringstream comments;
    static bool all();
    static bool DefaultTopicListsOKCrashOnFailure();
    static bool DefaultPdfsOKCrashOnFailure();
    bool DefaultPdfsOK();
    bool DefaultTopicListsOK();
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
  void ComputeID(int elementIndex, TopicElementParser& owner);
  void reset();
  friend std::ostream& operator << (std::ostream& output, const TopicElement& theElt) {
    output << theElt.toString();
    return output;
  }
  bool IsError();
  bool ProblemOpensIfAvailable(std::stringstream* commentsOnFailure);
  bool PdfsOpenIfAvailable(CalculatorHTML& owner, std::stringstream* commentsOnFailure);
  bool PdfSlidesOpenIfAvailable(CalculatorHTML& owner, std::stringstream* commentsOnFailure);
  bool PdfHomeworkOpensIfAvailable(CalculatorHTML& owner, std::stringstream* commentsOnFailure);
  std::string toString() const;
  JSData toJSON(CalculatorHTML& owner);
  void ComputeSlides(CalculatorHTML& owner);
  JSData ComputeSlidesJSON(CalculatorHTML& owner);
  void ComputeHomework(CalculatorHTML& owner);
  JSData ComputeHomeworkJSON(CalculatorHTML& owner);
  void ComputeLinks(CalculatorHTML& owner, bool plainStyle);
  TopicElement();
  void makeError(const std::string& message);
  bool MergeTopicLine(const TopicElementParser::TopicLine& input);
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
  int NumAttemptsToInterpret;
  int MaxInterpretationAttempts;
  int NumProblemsFound;
  int NumVideosFound;
  int NumVideosWithSlidesFound;
  int NumVideosHandwrittenFound;
  int NumSlidesFound;
  int NumHandwrittenSolutionsFound;
  int NumAnswerIdsMathquilled;
  List<int> randomSeedsIfInterpretationFails;
  bool flagIsExamHome;
  bool flagIsExamProblem;
  bool flagParentInvestigated;
  bool flagIsForReal;
  bool flagLoadedFromDB;
  bool flagTagHtmlPresent;
  bool flagTagHeadPresent;
  bool flagTagBodyPresent;
  bool flagDoPrependProblemNavigationBar;
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
  UserCalculator currentUseR;
  static std::string stringScoredQuizzes;
  static std::string stringPracticE;
  static std::string stringProblemLink;
  List<std::string> slidesSourcesHeaders;
  List<std::string> sourcesHomeworkHeaders;
  std::string fileName;
  std::string RelativePhysicalFileNameWithFolder;
  std::string inputHtml;
  struct answerLabels {
    static std::string properties;
    static std::string idPanel;
    static std::string answerHighlight;
    static std::string idMQSpan;
    static std::string idMQSpanLocation;
    static std::string idButtonContainer;
    static std::string MQpanelButtonOptions;
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
  MapList<std::string, std::string, MathRoutines::HashString> outputScripts;

  std::stringstream logCommandsProblemGeneratioN;
  std::string courseHome;
  std::string topicListJavascriptWithTag;

  static const std::string BugsGenericMessage;
  HashedList<std::string, MathRoutines::HashString> tagKeysNoValue;
  List<std::string> calculatorTopicElementNames;
  List<std::string> calculatorTagsRecordedLiterally;
  List<std::string> calculatorClasses;
  List<std::string> calculatorClassesAnswerFields;
  List<std::string> autoCompleteExtras;
  List<char> splittingChars;
  List<SyntacticElementHTML> eltsStack;
  List<SyntacticElementHTML> theContent;
  //  List<std::string> answerFirstCorrectSubmission;
  Selection studentTagsAnswered;
  ProblemData theProblemData;
  List<std::string> answerHighlights;
  int topicLectureCounter;
  std::string topicListContent;
  std::string topicListFileName;
  HashedList<std::string, MathRoutines::HashString> problemNamesNoTopics;
  TopicElementParser topics;
  MapList<std::string, std::string, MathRoutines::HashString> theScripts;
  List<std::string> databaseStudentSections;
  bool flagLoadedSuccessfully;
  bool flagLoadedClassDataSuccessfully;
  /////////////////
  int getAnswerIndex(const std::string& desiredAnswerId);
  bool CheckContent(std::stringstream* comments);
  bool CanBeMerged(const SyntacticElementHTML& left, const SyntacticElementHTML& right);
  bool loadMe(bool doLoadDatabase, const std::string& inputRandomSeed, std::stringstream* commentsOnFailure);
  bool LoadAndParseTopicList(std::stringstream& comments);
  bool LoadDatabaseInfo(std::stringstream& comments);
  std::string CleanUpFileName(const std::string& inputLink);
  void initAutocompleteExtras();
  void initBuiltInSpanClasses();
  bool parseHTML(std::stringstream* comments);
  bool parseHTMLPrepareCommands(std::stringstream* comments);
  bool IsSplittingChar(const std::string& input);
  void LoadFileNames();
  bool IsStateModifierApplyIfYes(SyntacticElementHTML& inputElt);
  bool extractAnswerIds(std::stringstream* comments);
  bool interpretHtml(std::stringstream* comments);
  bool interpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments);
  void ComputeProblemLabel();
  void ComputeBodyDebugString();
  std::string ToStringInterprettedCommands(Calculator& theInterpreter, List<SyntacticElementHTML>& theElements);
  void LogProblemGenerationObsolete(Calculator& theInterpreter);
  bool InterpretProcessExecutedCommands(
    Calculator& theInterpreter, List<SyntacticElementHTML>& theElements, std::stringstream& comments
  );
  bool prepareAnswerElements(std::stringstream& comments);
  bool interpretAnswerHighlights(std::stringstream& comments);
  bool InterpretAnswerElements(std::stringstream& comments);
  bool interpretOneAnswerElement(SyntacticElementHTML& inputOutput);
  bool PrepareAndExecuteCommands(Calculator& theInterpreter, std::stringstream* comments);
  std::string PrepareUserInputBoxes();
  bool PrepareCommandsAnswerOnGiveUp(Answer& theAnswer, std::stringstream* comments);
  bool PrepareCommentsBeforeSubmission(Answer& theAnswer, std::stringstream* comments);
  bool PrepareCommentsBeforeInterpretation(Answer& theAnswer, std::stringstream* comments);
  bool PrepareCommandsSolution(Answer& theAnswer, std::stringstream* comments);
  bool PrepareCommandsAnswer(Answer& theAnswer, std::stringstream* comments);
  bool PrepareCommandsGenerateProblem(std::stringstream* comments);
  std::string GetProblemHeaderEnclosure();
  std::string GetProblemHeaderWithoutEnclosure();
  bool SetTagClassFromOpenTag(SyntacticElementHTML& output);
  bool SetTagClassFromCloseTag(SyntacticElementHTML& output);
  bool StoreRandomSeedCurrent(std::stringstream* commentsOnFailure);
  bool PrepareCommands(std::stringstream* comments);
  std::string CleanUpCommandString(const std::string& inputCommand);
  void InterpretNotByCalculatorNotAnswer(SyntacticElementHTML& inputOutput);
  void InterpretIfAnswer(SyntacticElementHTML& inputOutput);
  std::string GetDeadlineNoInheritance(const std::string& id);
  std::string GetDeadline(
    const std::string& problemName,
    const std::string& sectionNumber,
    bool& outputIsInherited
  );
  bool MergeOneProblemAdminData(
    const std::string& inputProblemName,
    ProblemData& inputProblemInfo,
    std::stringstream& commentsOnFailure
  );
  //bool MergeProblemInfoInDatabaseURLed(std::string& incomingProblemInfo, std::stringstream& commentsOnFailure);
  bool MergeProblemWeightAndStore(
    std::string& incomingProblemInfo, std::stringstream* commentsOnFailure
  );
  bool MergeProblemDeadlineAndStore(
    std::string& incomingProblemInfo, std::stringstream* commentsOnFailure
  );
  bool MergeProblemWeight(
    const JSData& inputJSON,
    MapList<std::string, ProblemData, MathRoutines::HashString>& outputAppendProblemInfo,
    bool checkFileExistence,
    std::stringstream* commentsOnFailure
  );
  bool MergeProblemDeadline(
    const JSData& inputJSON,
    MapList<std::string, ProblemData, MathRoutines::HashString>& outputAppendProblemInfo,
    std::stringstream* commentsOnFailure
  );
  bool StoreProblemWeights(
    MapList<std::string, ProblemData, MathRoutines::HashString>& toStore,
    std::stringstream* commentsOnFailure
  );
  bool StoreProblemDeadlines(
    MapList<std::string, ProblemData, MathRoutines::HashString>& toStore,
    std::stringstream* commentsOnFailure
  );
  // bool LoadDeadlineInfoFromJSONStringAppend
  // (const std::string& inputJSONString,
  //   MapLisT<std::string, ProblemData, MathRoutines::hashString>& outputDeadlineInfo,
  //   std::stringstream& commentsOnFailure);
  //void StoreDeadlineInfoIntoJSON
  //(JSData& outputData,
  // MapLisT<std::string, ProblemData, MathRoutines::hashString>& inputProblemInfo);
  QuerySet ToQuerySetProblemWeights(MapList<std::string, ProblemData, MathRoutines::HashString>& inputProblemInfo);
  JSData ToJSONDeadlines(MapList<std::string, ProblemData, MathRoutines::HashString>& inputProblemInfo);
  std::string ToStringDeadline(
    const std::string& topicID, bool problemAlreadySolved, bool returnEmptyStringIfNoDeadline, bool isSection
  );
  void ComputeDeadlinesAllSections(TopicElement& inputOutput);
  void ComputeDeadlinesAllSectionsNoInheritance(TopicElement& inputOutput);
  JSData ToStringTopicListJSON();
  std::string ToStringProblemInfo(const std::string& theFileName, const std::string& stringToDisplay = "");
  static std::string ToStringLinkFromProblem(const std::string& theFileName, bool practiceMode = true, int randomSeed = - 1);
  std::string ToStringLinkFromFileName(const std::string& theFileName);
  std::string ToStringLinkCurrentAdmin(const std::string& displayString, bool setDebugFlag, bool includeRandomSeed);
  std::string ToStringCalculatorProblemSourceFromFileName(const std::string& theFileName);
  void InterpretGenerateLink(SyntacticElementHTML& inputOutput);
  std::string ToStringProblemScoreFull(const std::string& theFileName);
  std::string ToStringProblemScoreShort(const std::string& theFileName, bool& outputAlreadySolved);
  bool ComputeAnswerRelatedStrings(SyntacticElementHTML& inputOutput);
  void InterpretGenerateStudentAnswerButton(SyntacticElementHTML& inputOutput);
  bool PrepareSectionList(std::stringstream& commentsOnFailure);
  void InterpretManageClass(SyntacticElementHTML& inputOutput);
  void InterpretTopicList(SyntacticElementHTML& inputOutput);
  bool ComputeTopicListAndPointsEarned(std::stringstream& commentsOnFailure);
  void InterpretTableOfContents(SyntacticElementHTML& inputOutput);
  void InterpretProblemNavigationBar(SyntacticElementHTML& inputOutput);
  void InterpretAccountInformationLinks(SyntacticElementHTML& inputOutput);
  void InterpretJavascripts(SyntacticElementHTML& inputOutput);
  JSData GetJavascriptMathQuillBoxesForJSON();
  void LoadCurrentProblemItem(
    bool needToLoadDatabaseMayIgnore, const std::string& inputRandomSeed, std::stringstream* commentsOnFailure
  );
  void FigureOutCurrentProblemList(std::stringstream& comments);
  std::string LoadAndInterpretCurrentProblemItemJSON(
    bool needToLoadDatabaseMayIgnore, const std::string& desiredRandomSeed, std::stringstream* commentsOnFailure
  );
  static unsigned int hashFunction(const CalculatorHTML& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return MathRoutines::HashString(this->fileName);
  }
  bool operator==(const CalculatorHTML& other) const {
    return this->fileName == other.fileName;
  }
  std::string ToStringAllSectionDeadlines(const std::string& topicID, bool isSection);
  std::string ToStringOnEDeadlineFormatted(
    const std::string& topicID,
    const std::string& sectionNumber,
    bool problemAlreadySolved,
    bool returnEmptyStringIfNoDeadline,
    bool isSection
  );
  std::string ToStringCalculatorArgumentsForProblem(
    const std::string& requestType,
    const std::string& studentView,
    const std::string& studentSection = "",
    bool includeRandomSeedIfAppropriate = false
  ) const;
  std::string ToStringProblemNavigation() const;
  std::string ToStringExtractedCommands();
  std::string ToStringContent();
  std::string ToStringParsingStack(List<SyntacticElementHTML>& theStack);
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
      bool Run();
      std::string ToStringHTMLTableRow(int rowIndex);
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
    bool ComputeTotalFiles();
    static bool BuiltInMultiple(
      int inputFirstFileIndex,
      int inputFilesToInterpret,
      int inputRandomSeed,
      int numberOfRepetitions,
      std::stringstream* comments
    );
    bool BuiltIn(
      int inputFirstFileIndex,
      int inputFilesToInterpret,
      int inputRandomSeed
    );
    static bool BuiltInCrashOnFailure();
    static bool all();
    std::string ToStringSummary();
    std::string ToHTMLBuiltIn();
    std::string ToHTMLDebug();
  };
};

class Course {
public:
  std::string courseTemplate;
  std::string courseTopicsNoFolder;
  std::string title;
  std::string flagRoughDraft;
  bool IsEmpty();
  void reset();
  std::string courseTopicsWithFolder();
  std::string toString() const;
  JSData toJSON() const;
};

class CourseList {
public:
  List<Course> theCourses;
  std::string errorMessage;
  bool LoadFromString(const std::string& input);
  std::string toHtml();
  bool load();
  JSData toJSON();
};

#endif
