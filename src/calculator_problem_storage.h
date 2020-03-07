// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef CALCULATOR_PROBLEM_STORAGE_HEADER_ALREADY_INCLUDED
#define CALCULATOR_PROBLEM_STORAGE_HEADER_ALREADY_INCLUDED
#include "calculator_interface.h"
#include "general_time_date.h"
#include "general_database_system_independent.h"

class SyntacticElementHTML {
public:
  struct Tags{
  public:
    static std::string calculator;
    static std::string calculatorHidden;
    static std::string calculatorSolution;
    static std::string calculatorExamProblem;
    static std::string calculatorAnswer;
  };
  int indexInOwner;
  int commandIndex;
  std::string syntacticRole;
  std::string content;
  std::string tag;
  MapList<std::string, std::string, MathRoutines::HashString> properties;
  List<std::string> defaultKeysIfMissing;
  List<std::string> defaultValuesIfMissing;
  List<std::string> tagKeysWithoutValue;
  List<SyntacticElementHTML> children;
  bool flagUseDisplaystyleInMathMode;
  bool flagUseMathMode;
  bool flagUseMathSpan;
  std::string interpretedCommand;
  static int ParsingNumDummyElements;
  bool IsInterpretedByCalculatorDuringProblemGeneration();
  bool IsInterpretedByCalculatorDuringSubmission();
  bool IsInterpretedNotByCalculator();
  bool IsHidden();
  bool IsCalculatorHidden();
  bool IsCalculatorCommand();
  bool IsAnswer();
  bool IsSolution();
  bool IsAnswerElement(std::string* desiredAnswerId);
  bool IsCommentBeforeInterpretation();
  bool IsCommentBeforeSubmission();
  bool IsAnswerOnGiveUp();
  std::string GetKeyValue(const std::string& theKey) const;
  void SetKeyValue(const std::string& theKey, const std::string& theValue);
  void resetAllExceptContent();
  std::string ToStringInterpretedBody();
  std::string ToStringInterpretedHead();
  std::string ToStringTagAndContent();
  std::string ToStringOpenTag(const std::string& overrideTagIfNonEmpty, bool immediatelyClose = false);
  std::string ToStringCloseTag(const std::string& overrideTagIfNonEmpty);
  std::string GetTagClass();
  std::string ToStringDebug();
  SyntacticElementHTML() {
    this->flagUseDisplaystyleInMathMode = false;
    this->indexInOwner = - 1;
    this->commandIndex = - 1;
    this->flagUseMathMode = true;
    this->flagUseMathSpan = true;
  }
  SyntacticElementHTML(const std::string& inputContent) {
    this->flagUseDisplaystyleInMathMode = false;
    this->flagUseMathMode = true;
    this->flagUseMathSpan = true;
    this->content = inputContent;
    this->indexInOwner = - 1;
    this->commandIndex = - 1;
  }
  bool operator==(const std::string& other) {
    return this->content == other;
  }
  bool operator!=(const std::string& other) {
    return this->content != other;
  }
};

class Answer {
public:
  bool flagAutoGenerateSubmitButtons;
  bool flagAutoGenerateMQButtonPanel;
  bool flagAutoGenerateMQfield;
  bool flagAutoGenerateVerificationField;
  bool flagAutoGenerateButtonSolution;
  bool flagSolutionFound;
  int numSubmissions;
  int numCorrectSubmissions;
  std::string commandsCommentsBeforeSubmission;
  std::string commandsCommentsBeforeInterpretatioN;
  std::string commandsBeforeAnswer;
  std::string commandsBeforeAnswerNoEnclosuresForDEBUGGING;
  std::string commandVerificationOnly;
  std::string commandsSolutionOnly;
  std::string commandsNoEnclosureAnswerOnGiveUpOnly;
  List<SyntacticElementHTML> solutionElements;
  MapList<std::string, std::string, MathRoutines::HashString> properties;
  std::string answerId;
  std::string varAnswerId;
  std::string idVerificationSpan;
  std::string idAnswerPanel;
  std::string idButtonSubmit;
  std::string idButtonInterpret;
  std::string idButtonAnswer;
  std::string idButtonSolution;
  std::string javascriptPreviewAnswer;
  //std::string htmlMQjavascript;
  std::string htmlSpanVerifyAnswer;
  std::string htmlAnswerHighlight;
  //////////////////////////////////////
  std::string MQpanelButtonOptions;
  //////////////////////////////////////
  std::string varMQfield;
  std::string MQobject;
  std::string idSpanSolution;
  std::string idMQfielD;
  std::string idMQFieldLocation;
  std::string idMQButtonPanelLocation;
  std::string MQUpdateFunction;
  std::string currentAnswerURLed;
  std::string currentAnswerClean;
  std::string firstCorrectAnswerURLed;
  std::string firstCorrectAnswerClean;
  Answer() {
    this->numSubmissions = 0;
    this->numCorrectSubmissions = 0;
    this->flagAutoGenerateSubmitButtons = true;
    this->flagAutoGenerateMQButtonPanel = true;
    this->flagAutoGenerateMQfield = true;
    this->flagAutoGenerateVerificationField = true;
    this->flagAutoGenerateButtonSolution = true;
    this->flagSolutionFound = false;
  }
  std::string ToString();
};

class ProblemDataAdministrative {
public:
  MapList<std::string, std::string, MathRoutines::HashString> problemWeightsPerCoursE;
  MapList<std::string, std::string, MathRoutines::HashString> deadlinesPerSection;
  bool GetWeightFromCoursE(
    const std::string& theCourseNonURLed,
    Rational& output,
    std::string* outputAsGivenByInstructor = nullptr
  );
  std::string ToString() const;
};

class ProblemData {
private:
  int expectedNumberOfAnswersFromDB;
  int knownNumberOfAnswersFromHD;
public:
  friend std::ostream& operator << (std::ostream& output, const ProblemData& theData) {
    output << theData.ToString();
    return output;
  }
  bool flagRandomSeedGiven;
  unsigned int randomSeed;
  bool flagProblemWeightIsOK;
  Rational Points;
  ProblemDataAdministrative adminData;
  int numCorrectlyAnswered;
  int totalNumSubmissions;
  //  int numAnswersSought;
  std::string commandsGenerateProblem;
  std::string commandsGenerateProblemNoEnclosures;
  std::string commandsGenerateProblemLink;
  MapList<std::string, Answer, MathRoutines::HashString> theAnswers;
  List<std::string> inputNonAnswerIds;
  int getExpectedNumberOfAnswers(const std::string& problemName, std::stringstream& commentsOnFailure);
  void AddEmptyAnswerIdOnTop(const std::string& inputAnswerId);
  ProblemData();
  bool CheckConsistency() const;
  bool CheckConsistencyMQids() const;
  bool LoadFromOldFormat(const std::string& inputData, std::stringstream& commentsOnFailure);
  bool LoadFromJSON(const JSData& inputData, std::stringstream& commentsOnFailure);
  std::string StorE();
  JSData StoreJSON() const;
  std::string ToString() const;
  std::string ToStringAvailableAnswerIds();
};

class EmailRoutines {
public:
  std::string subject;
  std::string ccEmail;
  std::string toEmail;
  std::string emailContent;
  std::string smtpWithPort;
  static List<bool> recognizedEmailCharacters;
  static List<bool>& GetRecognizedEmailChars();
  EmailRoutines();
  //bool IsValidForMailgunCommand(std::stringstream* commentsOnFailure);
  static bool IsOKEmail(const std::string& input, std::stringstream* commentsOnError);
  bool SendEmailWithMailGun(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral,
    std::stringstream* commentsGeneralSensitive
  );
};

class TopicElement;
class UserCalculator : public UserCalculatorData {
public:
  MapList<std::string, ProblemData, MathRoutines::HashString> theProblemData;
  TimeWrapper authenticationCreationTime;
  Rational pointsEarned;
  Rational pointsMax;
  void ComputeExpectedNumberOfAnswersPerProblem();
  void ComputePointsEarned(
    const HashedList<std::string, MathRoutines::HashString>& gradableProblems,
    MapList<std::string, TopicElement, MathRoutines::HashString>* theTopics,
    std::stringstream& commentsOnFailure
  );
  void SetProblemData(const std::string& problemName, const ProblemData& inputData);
  bool flagNewAuthenticationTokenComputedUserNeedsIt;
  bool InterpretDatabaseProblemDatA(const std::string& theInfo, std::stringstream& commentsOnFailure);
  bool InterpretDatabaseProblemDataJSON(const JSData& theData, std::stringstream& commentsOnFailure);
  bool StoreProblemData(const std::string& fileName, std::stringstream* commentsOnFailure);
  std::string GetSelectedRowEntry(const std::string& theKey);
  std::string GetMySQLclauseIdentifyingUserByEmailOrID();
  bool LoadFromDB(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral = nullptr);
  bool FetchOneColumn(
    const std::string& columnNameUnsafe, std::string& outputUnsafe, std::stringstream* failureComments = nullptr
  );
  void FetchColumns();
  bool AuthenticateWithUserNameAndPass(std::stringstream* commentsOnFailure);
  bool AuthenticateWithToken(std::stringstream* commentsOnFailure);
  bool Authenticate(std::stringstream* commentsOnFailure);
  std::string FirstLoginMessage();
  bool ShouldCommentOnMissingUser();
  bool ResetAuthenticationToken(std::stringstream* commentsOnFailure);
  bool SetPassword(std::stringstream* commentsOnFailure);
  bool Iexist(std::stringstream* comments);
  bool StoreToDB(bool doSetPassword, std::stringstream* commentsOnFailure);
  static bool IsAcceptableDatabaseInpuT(const std::string& input, std::stringstream* comments);
  static bool IsAcceptableCharDatabaseInpuT(char theChar);
  bool ComputeAndStoreActivationToken(std::stringstream* commentsOnFailure);
  void ComputeHashedSaltedPassword();
  bool GetActivationAbsoluteAddress(std::string& output, std::stringstream& comments);
  bool GetActivationAddress(std::string& output, const std::string& calculatorBase, std::stringstream& comments);
  static std::string GetActivationAddressFromActivationToken(
    const std::string& theActivationToken,
    const std::string& calculatorBase,
    const std::string& inputUserNameUnsafe,
    const std::string& inputEmailUnsafe
  );
  bool ComputeAndStoreActivationEmailAndTokens(
    std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
  );
  bool ComputeAndStoreActivationStats(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  std::string ToString();
  std::string ToStringSelectedColumns();
  UserCalculator();
  ~UserCalculator();
};


#endif // CALCULATOR_PROBLEM_STORAGE_HEADER_ALREADY_INCLUDED
