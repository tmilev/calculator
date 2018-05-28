//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderHtmlInterpretationAlreadyDefined
#define vpfHeaderHtmlInterpretationAlreadyDefined
#include "vpfHeader1General0_General.h"
#include "vpfHeader7DatabaseInterface.h"
#include <ctime>

static ProjectInformationInstance ProjectInfoHeaderHtmlInterpretation(__FILE__, "Html interpretation.");

class CalculatorHTML;

struct TopicElement
{
public:
  static int scoreButtonCounter;
  int type;
  enum topicType {tChapter = 1, tSection = 2, tSubSection = 3, tProblem = 4, tError, tTexHeader, tUndefined};
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
  std::string slidesProjector;
  std::string handwrittenSolution;
  std::string linkSlidesTex;
  std::string linkHomeworkTex;
  List<std::string> sourceSlides;
  List<std::string> sourceHomework;
  List<bool> sourceHomeworkIsSolution;
  std::string slidesPrintable;
  std::string problem;
  std::string error;
  std::string displayTitle;
  std::string displayVideoLink;
  std::string displayVideoHandwrittenLink;
  std::string displaySlidesLink;
  std::string displaySlidesPrintableLink;
  std::string displayHandwrittenSolution;
  std::string displayProblemLink;
  std::string displayResourcesLinks;
  std::string displayDeadlinE;
  std::string displayScore;
  std::string displayModifyWeight;
  std::string displayModifyDeadline;
  std::string studentScoresSpanId;
  Rational totalPointsEarned;
  Rational pointsEarnedInProblemsThatAreImmediateChildren;
  Rational maxPointsInAllChildren;
  //  Rational numAnsweredInAllChildren;
  //  Rational maxCorrectAnswersInAllChildren;
  void ComputeID();
  void reset(int parentSize, MapLisT<std::__cxx11::string, TopicElement, MathRoutines::hashString> *containerElements);
  friend std::ostream& operator << (std::ostream& output, const TopicElement& theElt)
  { output << theElt.ToString();
    return output;
  }
  std::string GetItemStart(CalculatorHTML& owner, bool doIncludeScoreButton, bool plainStyle);
  std::string ToStringStudentScoreButton();
  std::string ToStringStudentScoreReportPanel();
  std::string ToString()const;
  JSData ToJSON(CalculatorHTML& owner);
  std::string GetItemFinish(CalculatorHTML& owner);
  void ComputeLinks(CalculatorHTML& owner, bool plainStyle);
  TopicElement()
  { this->reset(0, 0);
  }
  static void GetTopicList
  (const std::string& inputString, MapLisT<std::string, TopicElement, MathRoutines::hashString>& output,
   CalculatorHTML& owner);
  static bool LoadTopicBundle
  (const std::string& inputFileName, MapLisT<std::string, List<std::string>, MathRoutines::hashString>& output,
   CalculatorHTML& owner, std::stringstream& errorStream);
  static void AddTopic(TopicElement& inputElt, MapLisT<std::string, TopicElement, MathRoutines::hashString>& output);
};

struct ProblemResources{
public:
  std::string video;
  std::string slides;
  std::string slidesPrintable;
  std::string deadline;
  Rational pointsScored;
  Rational maxPoints;
};

class CalculatorHTML
{
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
  bool flagDoPrependCalculatorNavigationBar;
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
#ifdef MACRO_use_MongoDB
  UserCalculator currentUseR;
#endif
  static std::string stringScoredQuizzes;
  static std::string stringPracticE;
  static std::string stringProblemLink;
  List<std::string> loadedTopicBundles;
  List<std::string> slidesSourcesHeaders;
  List<std::string> sourcesHomeworkHeaders;
  std::string fileName;
  std::string RelativePhysicalFileNameWithFolder;
  std::string inputHtml;

  std::string outputHtmlBodyNoTag;
  std::string outputHtmlHeadNoTag;
  std::string outputProblemNavigatioN;
  std::string outputDeadlineString;
  std::string outputProblemLabel;
  std::string outputProblemTitle;
  MapLisT<std::string, std::string, MathRoutines::hashString> outputScripts;

  std::stringstream logCommandsProblemGeneratioN;
  std::string courseHome;
  std::string topicListJavascriptWithTag;

  static const std::string BugsGenericMessage;
  HashedList<std::string, MathRoutines::hashString> tagKeysNoValue;
  HashedList<std::string, MathRoutines::hashString> calculatorTopicBundles;
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
  HashedList<std::string, MathRoutines::hashString> problemNamesNoTopics;
  MapLisT<std::string, TopicElement, MathRoutines::hashString> theTopicS;
  MapLisT<std::string, std::string, MathRoutines::hashString> theScripts;
  List<std::string> databaseStudentSections;
  bool flagLoadedSuccessfully;
  bool flagLoadedClassDataSuccessfully;
  /////////////////
  std::stringstream comments;
  int GetAnswerIndex(const std::string& desiredAnswerId);
  bool CheckContent(std::stringstream& comments);
  bool CheckConsistencyTopics();
  bool CanBeMerged(const SyntacticElementHTML& left, const SyntacticElementHTML& right);
  bool LoadMe(bool doLoadDatabase, std::stringstream& comments, const std::string& inputRandomSeed);
  bool LoadAndParseTopicList(std::stringstream& comments);
  bool LoadDatabaseInfo(std::stringstream& comments);
  std::string GetSectionSelector();
  std::string CleanUpFileName(const std::string& inputLink);
  void initAutocompleteExtras();
  void initBuiltInSpanClasses();
  void initTopicElementNames();
  bool ParseHTML(std::stringstream& comments);
  bool ParseHTMLPrepareCommands(std::stringstream& comments);
  bool IsSplittingChar(const std::string& input);
  void LoadFileNames();
  bool IsStateModifierApplyIfYes(SyntacticElementHTML& inputElt);
  bool ExtractAnswerIds(std::stringstream& comments);
  bool InterpretHtml(std::stringstream& comments);
  bool InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments);
  std::string ToStringInterprettedCommands(Calculator& theInterpreter, List<SyntacticElementHTML>& theElements);
  void LogProblemGenerationObsolete(Calculator& theInterpreter);
  bool InterpretProcessExecutedCommands(Calculator& theInterpreter, List<SyntacticElementHTML>& theElements, std::stringstream& comments);
  bool PrepareAnswerElements(std::stringstream& comments);
  bool InterpretAnswerHighlights(std::stringstream& comments);
  bool InterpretAnswerElements(std::stringstream& comments);
  bool InterpretOneAnswerElement(SyntacticElementHTML& inputOutput);
  bool PrepareAndExecuteCommands(Calculator& theInterpreter, std::stringstream& comments);
  std::string PrepareUserInputBoxes();
  bool PrepareCommandsAnswerOnGiveUp(Answer& theAnswer, std::stringstream& comments);
  bool PrepareCommentsBeforeSubmission(Answer& theAnswer, std::stringstream& comments);
  bool PrepareCommentsBeforeInterpretation(Answer& theAnswer, std::stringstream& comments);
  bool PrepareCommandsSolution(Answer& theAnswer, std::stringstream& comments);
  bool PrepareCommandsAnswer(Answer& theAnswer, std::stringstream& comments);
  bool PrepareCommandsGenerateProblem(std::stringstream& comments);
  std::string GetProblemHeaderEnclosure();
  std::string GetProblemHeaderWithoutEnclosure();
  bool SetTagClassFromOpenTag(SyntacticElementHTML& output);
  bool SetTagClassFromCloseTag(SyntacticElementHTML& output);
  bool StoreRandomSeedCurrent(std::stringstream& commentsOnFailure);
  bool PrepareCommands(std::stringstream& comments);
  std::string CleanUpCommandString(const std::string& inputCommand);
  void InterpretNotByCalculatorNotAnswer(SyntacticElementHTML& inputOutput);
  void InterpretIfAnswer(SyntacticElementHTML& inputOutput);
  std::string GetDeadline
  (const std::string& problemName, const std::string& sectionNumber, bool& outputIsInherited);
  bool MergeOneProblemAdminData
  (const std::string& inputProblemName, ProblemData& inputProblemInfo, std::stringstream& commentsOnFailure);
  bool MergeProblemInfoInDatabase(std::string& incomingProblemInfo, std::stringstream& commentsOnFailure);
  bool LoadProblemInfoFromURLedInputAppend
  (const std::string& inputInfoString,
   MapLisT<std::string, ProblemData, MathRoutines::hashString>& outputProblemInfo,
   std::stringstream& commentsOnFailure);
  bool LoadProblemInfoFromJSONAppend
  (const JSData& inputJSON,
   MapLisT<std::string, ProblemData, MathRoutines::hashString>& outputProblemInfo,
   std::stringstream& commentsOnFailure);
  //  bool LoadDeadlineInfoFromJSONStringAppend
  //  (const std::string& inputJSONString,
  //   MapLisT<std::string, ProblemData, MathRoutines::hashString>& outputDeadlineInfo,
  //   std::stringstream& commentsOnFailure);
  void StoreDeadlineInfo
  (JSData& outputData,
   MapLisT<std::string, ProblemData, MathRoutines::hashString>& inputProblemInfo);
  JSData ToJSONProblemWeights
  (MapLisT<std::string, ProblemData, MathRoutines::hashString>& inputProblemInfo);
  JSData ToJSONDeadlines
  (MapLisT<std::string, ProblemData, MathRoutines::hashString>& inputProblemInfo);
  std::string ToStringDeadline
  (const std::string& topicID, bool problemAlreadySolved, bool returnEmptyStringIfNoDeadline, bool isSection);
  void ComputeDeadlineModifyButton
  (TopicElement& inputOutput, bool problemAlreadySolved, bool isProblemGroup);
  std::string ToStringTopicListJSON();
  std::string ToStringProblemInfo(const std::string& theFileName, const std::string& stringToDisplay = "");
  std::string ToStringLinkFromFileName(const std::string& theFileName);
  std::string ToStringLinkCurrentAdmin
  (const std::string& displayString, bool setDebugFlag, bool includeRandomSeed);
  std::string ToStringCalculatorProblemSourceFromFileName(const std::string& theFileName);
  void InterpretGenerateLink(SyntacticElementHTML& inputOutput);
  std::string ToStringProblemWeightButton(const std::string& theFileName);
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
  std::string GetEditPagePanel();
  void InterpretEditPagePanel(SyntacticElementHTML& inputOutput);
  void InterpretProblemNavigationBar(SyntacticElementHTML& inputOutput);
  void InterpretCalculatorNavigationBar(SyntacticElementHTML& inputOutput);
  void InterpretAccountInformationLinks(SyntacticElementHTML& inputOutput);
  void InterpretJavascripts(SyntacticElementHTML& inputOutput);
  std::string GetEditPageButton(const std::string& desiredFileName, bool includeCloneButton = true);
  std::string GetJavascriptSubmitAnswers();
  std::string GetJavascriptMathQuillBoxes();
  JSData GetJavascriptMathQuillBoxesForJSON();
  void LoadCurrentProblemItem(bool needToLoadDatabaseMayIgnore, const std::string& inputRandomSeed);
  void FigureOutCurrentProblemList(std::stringstream& comments);
  std::string LoadAndInterpretCurrentProblemItem(bool needToLoadDatabaseMayIgnore, const std::string& desiredRandomSeed);
  std::string LoadAndInterpretCurrentProblemItemJSON(bool needToLoadDatabaseMayIgnore, const std::string& desiredRandomSeed);
  static unsigned int HashFunction(const CalculatorHTML& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return MathRoutines::hashString(this->fileName);
  }
  bool operator==(const CalculatorHTML& other)const
  { return this->fileName == other.fileName;
  }
  std::string ToStringAllSectionDeadlines(const std::string& topicID, bool isSection);
  std::string ToStringOnEDeadlineFormatted
  (const std::string& topicID, const std::string& sectionNumber,
  bool problemAlreadySolved, bool returnEmptyStringIfNoDeadline, bool isSection);
  std::string ToStringCalculatorArgumentsForProblem
  (const std::string& requestType, const std::string& studentView,
   const std::string& studentSection = "", bool includeRandomSeedIfAppropriate = false)const;
  std::string ToStringProblemNavigation()const;
  std::string ToStringExtractedCommands();
  std::string ToStringContent();
  std::string ToStringParsingStack(List<SyntacticElementHTML>& theStack);
  CalculatorHTML();
};


#endif
