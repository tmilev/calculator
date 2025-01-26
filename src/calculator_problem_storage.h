#ifndef header_calculator_problem_storage_ALREADY_INCLUDED
#define header_calculator_problem_storage_ALREADY_INCLUDED

#include "general_maps.h"
#include "json.h"
#include "math_large_integers.h"

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
    static std::string commentsBeforeSubmission;
  };

  int indexInOwner;
  int commandIndex;
  std::string syntacticRole;
  std::string content;
  std::string tag;
  std::string errorComment;
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >
  properties;
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
    const std::string& overrideTagIfNonEmpty, bool immediatelyClose =
    false
  ) const;
  std::string toStringCloseTag(const std::string& overrideTagIfNonEmpty) const;
  std::string getTagClass() const;
  std::string toStringDebug() const;
  static std::string toHTMLElements(const List<SyntacticElementHTML>& input);
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
  // Intended for debugging problem generation.
  std::string commandsBeforeAnswerNoEnclosuresInternal;
  std::string commandVerificationOnly;
  std::string commandsSolutionOnly;
  std::string commandAnswerOnGiveUp;
  List<SyntacticElementHTML> solutionElements;
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >
  properties;
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
  std::string inputPanelOptions;
  // ////////////////////////////////////
  std::string idSpanSolution;
  std::string idMathEquationField;
  std::string idMathButtonPanelLocation;
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
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >
  problemWeightsPerCourse;
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >
  deadlinesPerSection;
  bool getWeightFromCourse(
    const std::string& courseNonURLed,
    Rational& output,
    std::string* outputAsGivenByInstructor =
    nullptr
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
  MapList<std::string, Answer, HashFunctions::hashFunction<std::string> >
  answers;
  List<std::string> inputNonAnswerIds;
  int getExpectedNumberOfAnswers(
    const std::string& problemName, std::stringstream& commentsOnFailure
  );
  void readExpectedNumberOfAnswersFromDatabase(
    std::stringstream& commentsOnFailure
  );
  ProblemData();
  bool checkConsistency() const;
  bool checkConsistencyMathIds() const;
  bool loadFromOldFormatDeprecated(
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

#endif // header_calculator_problem_storage_ALREADY_INCLUDED
