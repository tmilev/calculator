//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderHtmlInterpretationAlreadyDefined
#define vpfHeaderHtmlInterpretationAlreadyDefined
#include "vpfHeader1General0_General.h"
#include "vpfHeader7DatabaseInterface_MySQL.h"
#include <ctime>

static ProjectInformationInstance ProjectInfoHeaderHtmlInterpretation(__FILE__, "Html interpretation.");

class CalculatorHTML
{
public:
  int NumAttemptsToInterpret;
  int MaxInterpretationAttempts;
  int NumProblemsFound;
  int NumAnswerIdsMathquilled;
  List<int> randomSeedsIfInterpretationFails;
  bool flagRandomSeedGiven;
  bool flagIsExamHome;
  bool flagIsExamProblem;
  bool flagParentInvestigated;
  bool flagIsForReal;
  bool flagLoadedFromDB;
  double timeToParseHtml;
  List<double> timePerAttempt;
  List<List<double> > timeIntermediatePerAttempt;
  List<List<std::string> > timeIntermediateComments;
#ifdef MACRO_use_MySQL
  UserCalculator currentUser;
#endif
  std::string fileName;
  std::string RelativePhysicalFileNameWithFolder;
  std::string inputHtml;
  std::string outputHtmlMain;
  std::string outputHtmlNavigation;
  std::string currentExamHomE;
  std::string logCommandsProblemGeneration;
  static const std::string BugsGenericMessage;
  HashedList<std::string, MathRoutines::hashString> tagKeysNoValue;
  List<std::string> calculatorClasses;
  List<std::string> calculatorClassesAnswerFields;
  List<char> splittingChars;
  List<SyntacticElementHTML> eltsStack;
  List<SyntacticElementHTML> theContent;
//  List<std::string> answerFirstCorrectSubmission;
  Selection studentTagsAnswered;
  ProblemData theProblemData;
  HashedList<std::string, MathRoutines::hashString> hdProblemList;
  List<std::string> hdHomeworkGroupCorrespondingToEachProblem;
  List<List<std::string> > hdHomeworkGroups;
  HashedList<std::string, MathRoutines::hashString> hdHomeworkGroupNames;
  HashedList<std::string, MathRoutines::hashString> databaseProblemAndHomeworkGroupList;
//  List<std::string> databaseHomeworkGroupCorrespondingToEachProblem;
  List<List<std::string> > databaseHomeworkGroupDeadlinesPerSection;
  List<List<std::string> > databaseHomeworkGroupMaxNumTriesPerSection;
  List<std::string> databaseProblemWeights;
  List<List<std::string> > databaseStudentSectionsPerProblem;
  List<List<std::string> > databaseDeadlinesBySection;
  List<std::string> databaseStudentSectionS;
  List<List<std::string> > userTablE;
  List<std::string> labelsUserTablE;
  List<std::string> problemListOfParent;
  std::string currentUserDatabaseString;
  std::string currentProblemCollectionDatabaseString;
  bool flagLoadedSuccessfully;
  bool flagLoadedClassDataSuccessfully;
  /////////////////
  std::stringstream comments;
  int GetAnswerIndex(const std::string& desiredAnswerId);
  bool CheckContent(std::stringstream& comments);
  bool CanBeMerged(const SyntacticElementHTML& left, const SyntacticElementHTML& right);
  bool LoadMe(bool doLoadDatabase, std::stringstream& comments);
  bool LoadDatabaseInfo(std::stringstream& comments);
  std::string CleanUpFileName(const std::string& inputLink);
  bool ParseHTMLComputeChildFiles(std::stringstream& comments);
  bool ParseHTML(std::stringstream& comments);
  bool ParseHTMLPrepareCommands(std::stringstream& comments);
  bool IsSplittingChar(const std::string& input);
  void LoadFileNames();
  bool IsStateModifierApplyIfYes(SyntacticElementHTML& inputElt);
  bool ExtractAnswerIds(std::stringstream& comments);
  bool InterpretHtml(std::stringstream& comments);
  bool InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments);
  std::string ToStringInterprettedCommands(Calculator& theInterpreter, List<SyntacticElementHTML>& theElements);
  bool ProcessInterprettedCommands(Calculator& theInterpreter, List<SyntacticElementHTML>& theElements, std::stringstream& comments);
  bool PrepareAnswerElements(std::stringstream &comments);
  bool InterpretAnswerElements(std::stringstream& comments);
  bool InterpretOneAnswerElement(SyntacticElementHTML& inputOutput);
  bool PrepareAndExecuteCommands(Calculator& theInterpreter, std::stringstream& comments);
  std::string PrepareUserInputBoxes();
  bool PrepareCommandsAnswerOnGiveUp(Answer& theAnswer, std::stringstream& comments);
  bool PrepareCommentsBeforeSubmission(Answer& theAnswer, std::stringstream& comments);
  bool PrepareCommandsSolution(Answer& theAnswer, std::stringstream& comments);
  bool PrepareCommandsAnswer(Answer& theAnswer, std::stringstream& comments);
  bool PrepareCommandsGenerateProblem(std::stringstream& comments);
  std::string ProcessAnswerGiveUp();
  std::string GetProblemHeaderEnclosure();
  bool PrepareCommands(std::stringstream& comments);
  std::string CleanUpCommandString(const std::string& inputCommand);
  void InterpretNotByCalculator(SyntacticElementHTML& inputOutput);
  std::string GetDeadline
  (
   const std::string& problemName
   //int indexInDatabase
   ,
   const std::string& sectionNumber
   , bool inheritFromGroup, bool& outputIsInherited);
  std::string InterpretGenerateDeadlineLink
(SyntacticElementHTML& inputOutput,
 const std::string& cleaneduplink, const std::string& urledProblem, bool problemAlreadySolved)
  ;
  std::string ToStringLinkFromFileName(const std::string& theFileName);
  std::string ToStringCalculatorProblemSourceFromFileName(const std::string& theFileName);
  void InterpretGenerateLink(SyntacticElementHTML& inputOutput);
  std::string InterpretGenerateProblemManagementLink
(std::stringstream& refStreamForReal, std::stringstream& refStreamExercise,
 const std::string& cleaneduplink, const std::string& urledProblem)
  ;
  bool ComputeAnswerRelatedStrings(SyntacticElementHTML& inputOutput);
  void InterpretGenerateStudentAnswerButton(SyntacticElementHTML& inputOutput);
  bool PrepareClassData(std::stringstream& commentsOnFailure);
  void InterpretManageClass(SyntacticElementHTML& inputOutput);
  std::string ToStringClassDetails
( bool adminsOnly,
  const SyntacticElementHTML& inputElement
)
  ;
  std::string GetEditPageButton();
  std::string GetJavascriptSubmitMainInputIncludeCurrentFile();
  std::string GetDatePickerJavascriptInit();
  std::string GetDatePickerStart(const std::string& theId);
  std::string GetJavascriptSubmitAnswers();
  void LoadCurrentProblemItem();
  void FigureOutCurrentProblemList(std::stringstream& comments);
  std::string LoadAndInterpretCurrentProblemItem();
  bool FindExamItem();
  static unsigned int HashFunction(const CalculatorHTML& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return MathRoutines::hashString(this->fileName);
  }
  bool operator==(const CalculatorHTML& other)const
  { return this->fileName==other.fileName;
  }
  std::string ToStringDeadlinesFormatted
  (const std::string& cleanedUpLink, const List<std::string>& sectionNumbers, bool isActualProblem, bool problemAlreadySolved)
  ;
  std::string ToStringOnEDeadlineFormatted
  (const std::string& cleanedUpLink, const std::string& sectionNumber, bool isActualProblem, bool problemAlreadySolved)
  ;
  std::string ToStringCalculatorArgumentsForProblem
  (const std::string& requestType, const std::string& studentView,
   const std::string& studentSection="", bool includeRandomSeedIfAppropriate=false)const;
  std::string ToStringProblemNavigation()const;
  std::string ToStringExtractedCommands();
  std::string ToStringContent();
  std::string ToStringParsingStack(List<SyntacticElementHTML>& theStack);
  CalculatorHTML();
};


#endif
