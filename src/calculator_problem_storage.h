
#ifndef header_calculator_problem_storage_ALREADY_INCLUDED
#define header_calculator_problem_storage_ALREADY_INCLUDED

#include "calculator_interface.h"
#include "general_time_date.h"
#include "general_database_system_independent.h"

class SyntacticElementHTML {
public:
  struct Tags {
  public:
    static std::string filler;
    static std::string command;
    static std::string calculator;
    static std::string calculatorHidden;
    static std::string calculatorSolution;
    static std::string calculatorShowToUserOnly;
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
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > properties;
  List<std::string> propertiesWithoutValue;
  List<std::string> defaultKeysIfMissing;
  List<std::string> defaultValuesIfMissing;
  // List<SyntacticElementHTML> children;
  bool flagUseDisplaystyleInMathMode;
  bool flagUseMathMode;
  bool flagUseMathSpan;
  std::string interpretedCommand;
  static int parsingDummyElements;
  bool isInterpretedByCalculatorOnGeneration() const;
  bool isInterpretedByCalculatorOnSubmission();
  bool isInterpretedNotByCalculator();
  bool shouldShow() const;
  bool isCalculatorHidden();
  bool isCalculatorCommand() const;
  bool isCalculatorCommandGenerationOnly() const;
  bool isAnswerStandard() const;
  bool isAnswerHardCoded() const;
  bool isAnswer() const;
  bool isSolution();
  std::string getAnswerIdOfOwner() const;
  std::string answerIdIfAnswer() const;
  std::string answerIdCorrectIfEmpty();
  bool isAnswerElement(std::string* desiredAnswerId);
  bool isCommentBeforeInterpretation();
  bool isCommentBeforeSubmission();
  bool isAnswerOnGiveUp();
  std::string getKeyValue(const std::string& key) const;
  void setKeyValue(const std::string& key, const std::string& value);
  void resetAllExceptContent();
  std::string toStringInterpretedBody();
  std::string toStringTagAndContent() const;
  std::string toStringOpenTag(
    const std::string& overrideTagIfNonEmpty, bool immediatelyClose = false
  ) const;
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
  static std::string cleanUpCommandString(const std::string& inputCommand);
  static std::string cleanUpEncloseCommand(const std::string& inputCommand);
  std::string commandCleaned() const;
  std::string commandEnclosed() const;
};

class Answer {
private:
  bool prepareAnswerStandard(
    const SyntacticElementHTML& input,
    std::stringstream& commands,
    std::stringstream& commandsBody,
    std::stringstream& commandsNoEnclosures,
    std::stringstream& commandsBodyNoEnclosures
  );
  bool prepareAnswerHardCoded(
    const SyntacticElementHTML& input,
    std::stringstream& commands,
    std::stringstream& commandsBody,
    std::stringstream& commandsNoEnclosures,
    std::stringstream& commandsBodyNoEnclosures
  );
public:
  bool flagAnswerVerificationFound;
  bool flagAutoGenerateSubmitButtons;
  bool flagAutoGenerateMQButtonPanel;
  bool flagAutoGenerateMQfield;
  bool flagAutoGenerateVerificationField;
  bool flagAutoGenerateButtonSolution;
  bool flagAnswerHardcoded;
  int numberOfSubmissions;
  int numberOfCorrectSubmissions;
  std::string commandsCommentsBeforeSubmission;
  std::string commandsCommentsBeforeInterpretation;
  std::string commandsBeforeAnswer;
  std::string commandsBeforeAnswerNoEnclosuresForDEBUGGING;
  std::string commandVerificationOnly;
  std::string commandsSolutionOnly;
  std::string commandAnswerOnGiveUp;
  List<SyntacticElementHTML> solutionElements;
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > properties;
  std::string answerId;
  std::string idVerificationSpan;
  std::string idAnswerPanel;
  std::string idButtonSubmit;
  std::string idButtonInterpret;
  std::string idButtonAnswer;
  std::string idButtonSolution;
  std::string javascriptPreviewAnswer;
  // std::string htmlMQjavascript;
  std::string htmlSpanVerifyAnswer;
  std::string htmlAnswerHighlight;
  // ////////////////////////////////////
  std::string mathQuillPanelOptions;
  // ////////////////////////////////////
  std::string idSpanSolution;
  std::string idMathEquationField;
  std::string idMQFieldLocation;
  std::string idMQButtonPanelLocation;
  std::string currentAnswerURLed;
  std::string currentAnswerClean;
  std::string firstCorrectAnswerURLed;
  std::string firstCorrectAnswerClean;
  Answer();
  // Returns true if answer checking instructions could be extracted from the
  // input.
  bool prepareAnswer(
    const SyntacticElementHTML& input,
    std::stringstream& commands,
    std::stringstream& commandsBody,
    std::stringstream& commandsNoEnclosures,
    std::stringstream& commandsBodyNoEnclosures
  );
  bool hasSolution() const;
  std::string toString();
  std::string toStringSolutionElements();
};

class ProblemDataAdministrative {
public:
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > problemWeightsPerCourse;
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > deadlinesPerSection;
  bool getWeightFromCourse(
    const std::string& courseNonURLed,
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
  friend std::ostream& operator<<(
    std::ostream& output, const ProblemData& data
  ) {
    output << data.toString();
    return output;
  }
  bool flagRandomSeedGiven;
  uint32_t randomSeed;
  bool flagProblemWeightIsOK;
  Rational points;
  ProblemDataAdministrative adminData;
  int totalCorrectlyAnswered;
  int totalSubmissions;
  std::string commandsGenerateProblem;
  std::string commandsGenerateProblemNoEnclosures;
  std::string commandsGenerateProblemLink;
  MapList<
    std::string, Answer, HashFunctions::hashFunction<std::string>
  > answers;
  List<std::string> inputNonAnswerIds;
  int getExpectedNumberOfAnswers(
    const std::string& problemName, std::stringstream& commentsOnFailure
  );
  ProblemData();
  bool checkConsistency() const;
  bool checkConsistencyMathQuillIds() const;
  bool loadFromOldFormat(
    const std::string& inputData, std::stringstream& commentsOnFailure
  );
  bool loadFromJSON(
    const JSData& inputData, std::stringstream& commentsOnFailure
  );
  std::string store();
  JSData storeJSON() const;
  std::string toString() const;
  std::string toStringAvailableAnswerIds();
};

class EmailRoutines {
public:
  // Preconfigured web address of the present server.
  // Do not use the following web addresses to identify the server,
  // EXCEPT in special circumstances described below.
  // Instead, to get the web address of the
  // calculator server,
  // please use global.hostNoPort.
  // That address is extracted from the
  // incoming message headers
  // and is how our server is seen from the outside world.
  // HOWEVER,
  // the incoming message headers may be forged.
  // In most cases, this is not a problem
  // as the forgery will
  // affect negatively only the forgerer.
  // HOWEVER,
  // we CANNOT rely on the headers when sending
  // activation emails, as an attacker could claim my email,
  // forge his headers, and send me activation email which would
  // wrongly link to the host given in his forged headers
  // - that would be an evil site prompting me for password.
  // So, only in this very special circumstance,
  // we can only rely on a preconfigured web address.
  // For example, this could be "https://calculator-algebra.org".
  static std::string webAdress;
  static std::string sendEmailFrom;
  std::string subject;
  std::string ccEmail;
  std::string toEmail;
  std::string emailContent;
  std::string smtpWithPort;
  static List<bool> recognizedEmailCharacters;
  static List<bool>& getRecognizedEmailChars();
  EmailRoutines();
  // bool IsValidForMailgunCommand(std::stringstream* commentsOnFailure);
  static bool isOKEmail(
    const std::string& input, std::stringstream* commentsOnError
  );
  // Sends email with the mailgun api.
  // Required setup.
  // 1) Get an account with mailgun.
  // 2) Get a domain to send email from.
  // 3) Add the domain into configuration/configuration.json
  // in a format similar to:
  // {
  // ...
  // "sendEmailFrom": "mail2.the_domain_name_of_your_calculator_instance.org",
  // ...
  // }
  // 4) Create a file called
  // certificates/mailgun-api.txt
  // (in folder certificates/).
  // 5) Paste your secret mailgun api key
  // into that file.
  // You get the key from your mailgun registration.
  // The key needs to be installed in your server manually
  // (or through a secure orchestration service)
  // as it is a secret that allows anyone who has it
  // to send emails from your domain.
  // 6. As of writing, you also need to have curl installed on the same machine
  // as your server. We need curl to send the authorization key.
  //
  // Reason: as of writing,
  // the calculator does not implement the full
  // http(s) protocol - we only implement the large chunk of it needed
  // to run our complicated math workflow.
  // If you have a hard time installing curl, drop us a feature request
  // to remove the curl dependency.
  bool sendEmailWithMailGun(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral,
    std::stringstream* commentsGeneralSensitive
  );
};

class TopicElement;

class UserCalculator: public UserCalculatorData {
public:
  MapList<
    std::string,
    ProblemData,
    HashFunctions::hashFunction<std::string>
  > problemData;
  TimeWrapper authenticationCreationTime;
  Rational pointsEarned;
  Rational pointsMaximum;
  bool flagNewAuthenticationTokenComputedUserNeedsIt;
  void computeExpectedNumberOfAnswersPerProblem();
  void computePointsEarned(
    const HashedList<std::string>& gradableProblems,
    MapList<
      std::string,
      TopicElement,
      HashFunctions::hashFunction<std::string>
    >* topics,
    std::stringstream& commentsOnFailure
  );
  void setProblemData(
    const std::string& problemName, const ProblemData& inputData
  );
  bool interpretDatabaseProblemData(
    const std::string& information, std::stringstream& commentsOnFailure
  );
  bool interpretDatabaseProblemDataJSON(
    const JSData& data, std::stringstream& commentsOnFailure
  );
  bool storeProblemData(
    const std::string& fileName, std::stringstream* commentsOnFailure
  );
  std::string getSelectedRowEntry(const std::string& key);
  bool loadFromDatabase(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral = nullptr
  );
  bool fetchOneColumn(
    const std::string& columnNameUnsafe,
    std::string& outputUnsafe,
    std::stringstream* failureComments = nullptr
  );
  bool authenticateWithPassword(std::stringstream* commentsOnFailure);
  bool authenticateWithToken(std::stringstream* commentsOnFailure);
  bool authenticate(std::stringstream* commentsOnFailure);
  std::string firstLoginMessage();
  bool shouldCommentOnMissingUser();
  bool resetAuthenticationToken(std::stringstream* commentsOnFailure);
  bool setPassword(std::stringstream* commentsOnFailure);
  bool exists(std::stringstream* comments);
  bool storeToDatabase(
    bool doSetPassword, std::stringstream* commentsOnFailure
  );
  static bool isAcceptableDatabaseInput(
    const std::string& input, std::stringstream* comments
  );
  static bool isAcceptableCharDatabaseInput(char character);
  bool computeAndStoreActivationToken(std::stringstream* commentsOnFailure);
  void computeHashedSaltedPassword();
  bool getActivationAddress(
    std::string& output, std::stringstream& comments
  );
  static bool getActivationAddressFromActivationToken(
    const std::string& activationToken,
    const std::string& inputUserNameUnsafe,
    const std::string& inputEmailUnsafe,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  bool computeAndStoreActivationEmailAndTokens(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  bool computeAndStoreActivationStats(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  std::string toString();
  std::string toStringSelectedColumns();
  UserCalculator();
  ~UserCalculator();
};

#endif // header_calculator_problem_storage_ALREADY_INCLUDED
