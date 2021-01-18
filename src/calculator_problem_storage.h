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
    static std::string filler;
    static std::string command;
    static std::string calculator;
    static std::string calculatorHidden;
    static std::string calculatorSolution;
    static std::string calculatorSolutionStart;
    static std::string calculatorSolutionEnd;
    static std::string calculatorExamProblem;
    static std::string calculatorAnswer;
    static std::string hardCodedAnswer;
    static std::string answerCalculatorHighlight;
    static std::string answerCalculatorHighlightStart;
    static std::string answerCalculatorHighlightEnd;
  };
  int indexInOwner;
  int commandIndex;
  std::string syntacticRole;
  std::string content;
  std::string tag;
  std::string errorComment;
  MapList<std::string, std::string, MathRoutines::hashString> properties;
  List<std::string> propertiesWithoutValue;
  List<std::string> defaultKeysIfMissing;
  List<std::string> defaultValuesIfMissing;
  // List<SyntacticElementHTML> children;
  bool flagUseDisplaystyleInMathMode;
  bool flagUseMathMode;
  bool flagUseMathSpan;
  std::string interpretedCommand;
  static int parsingDummyElements;
  bool isInterpretedByCalculatorDuringProblemGeneration() const;
  bool isInterpretedByCalculatorDuringSubmission();
  bool isInterpretedNotByCalculator();
  bool isHidden();
  bool isCalculatorHidden();
  bool isCalculatorCommand();
  bool isAnswer();
  bool isSolution();
  std::string getAnswerIdOfOwner() const;
  bool isAnswerElement(std::string* desiredAnswerId);
  bool isCommentBeforeInterpretation();
  bool isCommentBeforeSubmission();
  bool isAnswerOnGiveUp();
  std::string getKeyValue(const std::string& theKey) const;
  void setKeyValue(const std::string& theKey, const std::string& theValue);
  void resetAllExceptContent();
  std::string toStringInterpretedBody();
  std::string toStringTagAndContent() const;
  std::string toStringOpenTag(const std::string& overrideTagIfNonEmpty, bool immediatelyClose = false) const;
  std::string toStringCloseTag(const std::string& overrideTagIfNonEmpty) const;
  std::string getTagClass() const;
  std::string toStringDebug() const;
  static std::string toHTMLElements(
    const List<SyntacticElementHTML>& input
  );
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
  bool flagAnswerVerificationFound;
  bool flagAutoGenerateSubmitButtons;
  bool flagAutoGenerateMQButtonPanel;
  bool flagAutoGenerateMQfield;
  bool flagAutoGenerateVerificationField;
  bool flagAutoGenerateButtonSolution;
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
  MapList<std::string, std::string, MathRoutines::hashString> properties;
  std::string answerId;
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
  std::string mathQuillPanelOptions;
  //////////////////////////////////////
  std::string idSpanSolution;
  std::string idMQfielD;
  std::string idMQFieldLocation;
  std::string idMQButtonPanelLocation;
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
    this->flagAnswerVerificationFound = false;
  }
  bool hasSolution() const;
  std::string toString();
  std::string toStringSolutionElements();
};

class ProblemDataAdministrative {
public:
  MapList<std::string, std::string, MathRoutines::hashString> problemWeightsPerCourse;
  MapList<std::string, std::string, MathRoutines::hashString> deadlinesPerSection;
  bool getWeightFromCourse(
    const std::string& theCourseNonURLed,
    Rational& output,
    std::string* outputAsGivenByInstructor = nullptr
  );
  std::string toString() const;
};

class ProblemData {
private:
  int expectedNumberOfAnswersFromDB;
  int knownNumberOfAnswersFromHD;
public:
  friend std::ostream& operator << (std::ostream& output, const ProblemData& theData) {
    output << theData.toString();
    return output;
  }
  bool flagRandomSeedGiven;
  uint32_t randomSeed;
  bool flagProblemWeightIsOK;
  Rational points;
  ProblemDataAdministrative adminData;
  int numCorrectlyAnswered;
  int totalNumSubmissions;
  std::string commandsGenerateProblem;
  std::string commandsGenerateProblemNoEnclosures;
  std::string commandsGenerateProblemLink;
  MapList<std::string, Answer, MathRoutines::hashString> answers;
  List<std::string> inputNonAnswerIds;
  int getExpectedNumberOfAnswers(const std::string& problemName, std::stringstream& commentsOnFailure);
  ProblemData();
  bool checkConsistency() const;
  bool checkConsistencyMathQuillIds() const;
  bool loadFromOldFormat(const std::string& inputData, std::stringstream& commentsOnFailure);
  bool loadFromJSON(const JSData& inputData, std::stringstream& commentsOnFailure);
  std::string store();
  JSData storeJSON() const;
  std::string toString() const;
  std::string toStringAvailableAnswerIds();
};

class EmailRoutines {
public:
  std::string subject;
  std::string ccEmail;
  std::string toEmail;
  std::string emailContent;
  std::string smtpWithPort;
  static List<bool> recognizedEmailCharacters;
  static List<bool>& getRecognizedEmailChars();
  EmailRoutines();
  //bool IsValidForMailgunCommand(std::stringstream* commentsOnFailure);
  static bool isOKEmail(const std::string& input, std::stringstream* commentsOnError);
  bool sendEmailWithMailGun(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral,
    std::stringstream* commentsGeneralSensitive
  );
};

class TopicElement;
class UserCalculator : public UserCalculatorData {
public:
  MapList<std::string, ProblemData, MathRoutines::hashString> problemData;
  TimeWrapper authenticationCreationTime;
  Rational pointsEarned;
  Rational pointsMax;
  bool flagNewAuthenticationTokenComputedUserNeedsIt;
  void computeExpectedNumberOfAnswersPerProblem();
  void computePointsEarned(
    const HashedList<std::string, MathRoutines::hashString>& gradableProblems,
    MapList<std::string, TopicElement, MathRoutines::hashString>* theTopics,
    std::stringstream& commentsOnFailure
  );
  void setProblemData(const std::string& problemName, const ProblemData& inputData);
  bool interpretDatabaseProblemData(const std::string& theInfo, std::stringstream& commentsOnFailure);
  bool interpretDatabaseProblemDataJSON(const JSData& theData, std::stringstream& commentsOnFailure);
  bool storeProblemData(const std::string& fileName, std::stringstream* commentsOnFailure);
  std::string GetSelectedRowEntry(const std::string& theKey);
  std::string GetMySQLclauseIdentifyingUserByEmailOrID();
  bool loadFromDatabase(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral = nullptr);
  bool fetchOneColumn(
    const std::string& columnNameUnsafe, std::string& outputUnsafe, std::stringstream* failureComments = nullptr
  );
  bool authenticateWithUserNameAndPass(std::stringstream* commentsOnFailure);
  bool authenticateWithToken(std::stringstream* commentsOnFailure);
  bool authenticate(std::stringstream* commentsOnFailure);
  std::string firstLoginMessage();
  bool shouldCommentOnMissingUser();
  bool resetAuthenticationToken(std::stringstream* commentsOnFailure);
  bool setPassword(std::stringstream* commentsOnFailure);
  bool exists(std::stringstream* comments);
  bool storeToDatabase(bool doSetPassword, std::stringstream* commentsOnFailure);
  static bool isAcceptableDatabaseInput(const std::string& input, std::stringstream* comments);
  static bool isAcceptableCharDatabaseInput(char theChar);
  bool computeAndStoreActivationToken(std::stringstream* commentsOnFailure);
  void computeHashedSaltedPassword();
  bool getActivationAbsoluteAddress(std::string& output, std::stringstream& comments);
  bool getActivationAddress(std::string& output, const std::string& calculatorBase, std::stringstream& comments);
  static std::string getActivationAddressFromActivationToken(
    const std::string& activationToken,
    const std::string& calculatorBase,
    const std::string& inputUserNameUnsafe,
    const std::string& inputEmailUnsafe
  );
  bool computeAndStoreActivationEmailAndTokens(
    std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
  );
  bool computeAndStoreActivationStats(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  std::string toString();
  std::string toStringSelectedColumns();
  UserCalculator();
  ~UserCalculator();
};

#endif // CALCULATOR_PROBLEM_STORAGE_HEADER_ALREADY_INCLUDED
