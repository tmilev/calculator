#ifndef header_calculator_user_ALREADY_INCLUDED
#define header_calculator_user_ALREADY_INCLUDED

#include "calculator_problem_storage.h"
#include "general_database_system_independent.h"
#include "general_time_date.h"
#include "math_large_integers.h"

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
  void exists(
    bool& outputExists, bool& databaseIsOK, std::stringstream* comments
  );
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

#endif // header_calculator_user_ALREADY_INCLUDED
