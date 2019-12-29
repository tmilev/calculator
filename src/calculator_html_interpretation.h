// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderHtmlInterpretationAlreadyDefined
#define vpfHeaderHtmlInterpretationAlreadyDefined
#include "general_lists.h"
#include "database.h"
#include "calculator_problem_storage.h"
#include <ctime>

static ProjectInformationInstance projectInfoHtmlInterpretationHeader(__FILE__, "Html interpretation.");

class CalculatorHTML;
class TopicElement;

class TopicElementParser {
public:
  class TopicLine {
  public:
    friend std::ostream& operator<<(std::ostream& output, const TopicLine& line) {
      output << line.ToString();
      return output;
    }
    int theType;
    std::string tag;
    std::string contentTrimmedWhiteSpace;
    void MakeError(const std::string& message);
    void MakeEmpty();
    TopicElement ToTopicElement() const;
    std::string ToString() const;
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
  bool CheckInitialization();
  bool CheckConsistencyParsed();
  void AddTopic(TopicElement& inputElt, int index);
  std::string ToString() const;
  void initializeElementTypes();
  TopicLine ExtractLine(const std::string& inputNonTrimmed);
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
    output << theElt.ToString();
    return output;
  }
  std::string GetItemStart(CalculatorHTML& owner, bool doIncludeScoreButton, bool plainStyle);
  std::string ToStringStudentScoreButton();
  std::string ToString() const;
  JSData ToJSON(CalculatorHTML& owner);
  std::string GetItemFinish(CalculatorHTML& owner);
  void ComputeSlides(CalculatorHTML& owner);
  void ComputeHomework(CalculatorHTML& owner);
  void ComputeLinks(CalculatorHTML& owner, bool plainStyle);
  TopicElement();
  void MakeError(const std::string& message);
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
  bool flagUseJSON;
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
  int GetAnswerIndex(const std::string& desiredAnswerId);
  bool CheckContent(std::stringstream* comments);
  bool CanBeMerged(const SyntacticElementHTML& left, const SyntacticElementHTML& right);
  bool LoadMe(bool doLoadDatabase, const std::string& inputRandomSeed, std::stringstream* commentsOnFailure);
  bool LoadAndParseTopicList(std::stringstream& comments);
  bool LoadDatabaseInfo(std::stringstream& comments);
  std::string GetSectionSelector();
  std::string CleanUpFileName(const std::string& inputLink);
  void initAutocompleteExtras();
  void initBuiltInSpanClasses();
  bool ParseHTML(std::stringstream* comments);
  bool ParseHTMLPrepareCommands(std::stringstream* comments);
  bool IsSplittingChar(const std::string& input);
  void LoadFileNames();
  bool IsStateModifierApplyIfYes(SyntacticElementHTML& inputElt);
  bool ExtractAnswerIds(std::stringstream* comments);
  bool InterpretHtml(std::stringstream* comments);
  bool InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments);
  void ComputeProblemLabel();
  void ComputeBodyDebugString();
  std::string ToStringInterprettedCommands(Calculator& theInterpreter, List<SyntacticElementHTML>& theElements);
  void LogProblemGenerationObsolete(Calculator& theInterpreter);
  bool InterpretProcessExecutedCommands(
    Calculator& theInterpreter, List<SyntacticElementHTML>& theElements, std::stringstream& comments
  );
  bool PrepareAnswerElements(std::stringstream& comments);
  bool InterpretAnswerHighlights(std::stringstream& comments);
  bool InterpretAnswerElements(std::stringstream& comments);
  bool InterpretOneAnswerElement(SyntacticElementHTML& inputOutput);
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
  void ComputeDeadlineModifyButton(TopicElement& inputOutput, bool problemAlreadySolved, bool isProblemGroup);
  JSData ToStringTopicListJSON();
  std::string ToStringProblemInfo(const std::string& theFileName, const std::string& stringToDisplay = "");
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
  void InterpretLectureMaterials(SyntacticElementHTML& inputOutput);
  void InterpretTableOfContents(SyntacticElementHTML& inputOutput);
  void InterpretProblemNavigationBar(SyntacticElementHTML& inputOutput);
  void InterpretAccountInformationLinks(SyntacticElementHTML& inputOutput);
  void InterpretJavascripts(SyntacticElementHTML& inputOutput);
  std::string GetEditPageButton(const std::string& desiredFileName, bool includeCloneButton = true);
  std::string GetJavascriptMathQuillBoxes();
  JSData GetJavascriptMathQuillBoxesForJSON();
  void LoadCurrentProblemItem(
    bool needToLoadDatabaseMayIgnore, const std::string& inputRandomSeed, std::stringstream* commentsOnFailure
  );
  void FigureOutCurrentProblemList(std::stringstream& comments);
  std::string LoadAndInterpretCurrentProblemItemJSON(
    bool needToLoadDatabaseMayIgnore, const std::string& desiredRandomSeed, std::stringstream* commentsOnFailure
  );
  static unsigned int HashFunction(const CalculatorHTML& input) {
    return input.HashFunction();
  }
  unsigned int HashFunction() const {
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
};

#endif
