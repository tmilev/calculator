//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader7_databaseMySQL_already_included
#define vpfHeader7_databaseMySQL_already_included
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader1General5TimeDate.h"
#include "vpfHeader1General8DatabaseSystemIndependent.h"

static ProjectInformationInstance ProjectInfoVpf8_1HeaderDatabaseInterface_MySQLx(__FILE__, "MySQL interface header. ");

class DatabaseRoutinesGlobalFunctions
{
public:
  static bool LogoutViaDatabase();
  static bool LoginViaDatabase(UserCalculatorData& theUseR, std::stringstream* comments);
  static bool LoginViaGoogleTokenCreateNewAccountIfNeeded
  (UserCalculatorData& theUseR, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  static bool SetPassword
(const std::string& inputUsername, const std::string& inputNewPassword, std::string& outputAuthenticationToken,
 std::stringstream& comments)
;
  static bool SetEntry
  (const std::string& inputUsername, const std::string& tableNameUnsafe, const std::string& keyNameUnsafe,
   const std::string& valueUnsafe, std::stringstream& comments)
   ;
  static bool FetchTablE
(List<List<std::string> >& output,
 List<std::string>& outputColumnLabels,
 bool& outputWasTruncated, int& actualNumRowsIfTruncated,
 const std::string& tableName, std::stringstream& comments)
;
  static bool FetchTableFromDatabaseIdentifier
(List<List<std::string> >& output,
 List<std::string>& outputColumnLabels,
 bool& outputWasTruncated, int& actualNumRowsIfTruncated,
 const std::string& tableIdentifier, std::stringstream& comments)
;
  static bool RowExists
  (const std::string& inputUsername, const std::string& tableName, std::stringstream& comments)
   ;
  static bool FetchEntry
  (const std::string& inputUsername, const std::string& tableName, const std::string& keyName,
   std::string& output, std::stringstream& comments);
  static bool TableExists
  (const std::string& tableName, std::stringstream& comments);
  static bool CreateTable
  (const std::string& tableName, std::stringstream& comments);
  static bool ColumnExists
  (const std::string& columnNameUnsafe, const std::string& tableNameUnsafe, std::stringstream& commentsStream);
  static bool CreateColumn
  (const std::string& columnNameUnsafe, const std::string& tableNameUnsafe,
   std::stringstream& commentsOnCreation);
   static bool UserDefaultHasInstructorRights();
};

class SyntacticElementHTML{
public:
  int indexInOwner;
  int commandIndex;
  std::string syntacticRole;
  std::string content;
  std::string tag;
  List<std::string> tagKeys;
  List<std::string> tagValues;
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
  std::string GetKeyValue(const std::string& theKey)const;
  void SetKeyValue(const std::string& theKey, const std::string& theValue);
  void resetAllExceptContent();
  std::string ToStringInterpretedBody();
  std::string ToStringInterpretedHead();
  std::string ToStringTagAndContent();
  std::string ToStringOpenTag(bool immediatelyClose=false);
  std::string ToStringCloseTag();
  std::string GetTagClass();
  std::string ToStringDebug();
  SyntacticElementHTML()
  { this->flagUseDisplaystyleInMathMode=false;
    this->indexInOwner=-1;
    this->commandIndex=-1;
    this->flagUseMathMode=true;
    this->flagUseMathSpan=true;
  }
  SyntacticElementHTML(const std::string& inputContent)
  { this->flagUseDisplaystyleInMathMode=false;
    this->flagUseMathMode=true;
    this->flagUseMathSpan=true;
    this->content=inputContent;
    this->indexInOwner=-1;
    this->commandIndex=-1;
  }
  bool operator==(const std::string& other)
  { return this->content==other;
  }
  bool operator!=(const std::string& other)
  { return this->content!=other;
  }
};

struct Answer
{
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
  std::string answerId;
  std::string varAnswerId;
  std::string idVerificationSpan;
  std::string javascriptPreviewAnswer;
  //std::string htmlMQjavascript;
  std::string htmlButtonInterpret;
  std::string htmlButtonAnswer;
  std::string htmlButtonSubmit;
  std::string htmlButtonSolution;
  std::string htmlTextareaLatexAnswer;
  std::string htmlSpanMQfield;
  std::string htmlSpanVerifyAnswer;
  std::string htmlSpanSolution;
  std::string htmlSpanMQButtonPanel;
  //////////////////////////////////////
  std::string varMQfield;
  std::string MQobject;
  std::string idSpanSolution;
  std::string idMQfield;
  std::string idMQButtonPanelLocation;
  std::string MQUpdateFunction;
  std::string currentAnswerURLed;
  std::string currentAnswerClean;
  std::string firstCorrectAnswerURLed;
  std::string firstCorrectAnswerClean;
  Answer()
  { this->numSubmissions=0;
    this->numCorrectSubmissions=0;
    this->flagAutoGenerateSubmitButtons=true;
    this->flagAutoGenerateMQButtonPanel=true;
    this->flagAutoGenerateMQfield=true;
    this->flagAutoGenerateVerificationField=true;
    this->flagAutoGenerateButtonSolution=true;
    this->flagSolutionFound=false;
  }
  std::string ToString();
};

struct ProblemDataAdministrative
{
public:
  MapLisT<std::string, std::string, MathRoutines::hashString> problemWeightsPerSectionDB;
  MapLisT<std::string, std::string, MathRoutines::hashString> deadlinesPerSection;
  bool GetWeightFromSection(const std::string& theSection, Rational& output, std::string* outputAsGivenByInstructor=0);
  std::string ToString()const;
};

struct ProblemData
{
public:
  friend std::ostream& operator << (std::ostream& output, const ProblemData& theData)
  { output << theData.ToString();
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
  List<Answer> theAnswers;
  List<std::string> inputNonAnswerIds;
  void AddEmptyAnswerIdOnTop(const std::string& inputAnswerId);
  int GetAnswerIdIndex(const std::string& inputAnswerId);
//  List<List<std::string> > allAnswers;
  ProblemData();
  bool CheckConsistency()const;
  bool CheckConsistencyMQids()const;
  bool LoadFrom(const std::string& inputData, std::stringstream& commentsOnFailure);
  std::string Store();
  std::string ToString()const;
  std::string ToStringAvailableAnswerIds();
};

#ifdef MACRO_use_MySQL
#include <mysql/mysql.h>

//Command for installation of mysql on Ubuntu:
//sudo apt-get install libmysqlclient-dev


//Command for installing mailx:
//Ubuntu:
//sudo apt-get install heirloom-mailx
//CentOS:
//sudo yum install mailx
class EmailRoutines
{
public:
  std::string subject;
  std::string ccEmail;
  std::string fromEmail;
  std::string fromEmailAuth;
  std::string toEmail;
  std::string emailContent;
  std::string smtpWithPort;
  static List<bool> recognizedEmailCharacters;
  static List<bool>& GetRecognizedEmailChars();
  EmailRoutines();
  //bool IsValidForMailgunCommand(std::stringstream* commentsOnFailure);
  static bool IsOKEmail(const std::string& input, std::stringstream* commentsOnError);
  std::string GetCommandToSendEmailWithMailX();
  bool SendEmailWithMailGun
  (std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral,
   bool includeEmailCommandInComments=false);
};

class TopicElement;
class DatabaseRoutines;
class UserCalculator : public UserCalculatorData
{
// Unsafe entries may contain arbitrary strings.
// Safe entries, when enclosed with "" in ANSI mode are guaranteed to be valid safe Database identifiers.
// In other words, safe entries are guaranteed to not allow bobby-tables-exploits-of-a-mom (https://xkcd.com/327/).
// Users of this code should not touch any safe entries directly.
// Instead users should only modify the unsafe entries.
// Those are internally (and automatically) converted to safe entries (stored in the private variables below), and only then stored in
// the database.
public:
  MapLisT<std::string, ProblemData, MathRoutines::hashString> theProblemData;
  TimeWrapper authenticationCreationTime;
  Rational pointsEarned;
  Rational pointsMax;

  void ComputePointsEarned
  (const HashedList<std::string, MathRoutines::hashString>& gradableProblems,
   MapLisT<std::string, TopicElement, MathRoutines::hashString>* theTopics
   );
  ProblemData& HasProblemData(const std::string& problemName);
  ProblemData& GetProblemDataAddIfNotPresent(const std::string& problemName);
  void SetProblemData(const std::string& problemName, const ProblemData& inputData);
  bool flagNewAuthenticationTokenComputedUserNeedsIt;
  bool LoadProblemStringFromDatabase(DatabaseRoutines& theRoutines, std::string& output, std::stringstream& commentsOnFailure);
  bool InterpretDatabaseProblemData(const std::string& theInfo, std::stringstream& commentsOnFailure);
  bool StoreProblemDataToDatabase(DatabaseRoutines& theRoutines, std::stringstream& commentsOnFailure);
  std::string GetSelectedRowEntry(const std::string& theKey);
  std::string GetMySQLclauseIdentifyingUserByEmailOrID();
  bool FetchOneUserRow
  (DatabaseRoutines& theRoutines, std::stringstream* failureStream);
  bool FetchOneColumn
  (const std::string& columnNameUnsafe, std::string& outputUnsafe,
   DatabaseRoutines& theRoutines, std::stringstream* failureComments=0);
  void FetchColumns(DatabaseRoutines& theRoutines);
  bool AuthenticateWithUserNameAndPass(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure);
  bool AuthenticateWithToken(std::stringstream* commentsOnFailure);
  bool Authenticate(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure);
  bool ResetAuthenticationToken(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure);
  std::string GetPassword(DatabaseRoutines& theRoutines);
  bool SetColumnEntry
  (const std::string& columnNameUnsafe, const std::string& theValueUnsafe,
   DatabaseRoutines& theRoutines, std::stringstream* failureComments=0);
  bool SetPassword(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure);
  bool DeleteMe(DatabaseRoutines& theRoutines, std::stringstream& commentsOnFailure);
  bool Iexist(DatabaseRoutines& theRoutines, std::stringstream* comments);
  bool IamPresentInTable(DatabaseRoutines& theRoutines, const std::string& tableNameUnsafe, std::stringstream* comments=0);
  bool CreateMeIfUsernameUnique(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure);
  static bool IsAcceptableDatabaseInpuT(const std::string& input, std::stringstream* comments);
  static bool IsAcceptableCharDatabaseInpuT(char theChar);
  bool getUserPassAndEmail(Calculator& theCommands, const Expression& input);
  bool getUserAndPass(Calculator& theCommands, const Expression& input);
  bool getUser(Calculator& theCommands, const Expression& input);
  bool getUserPassAndSelectedColumns(Calculator& theCommands, const Expression& input);
  bool getUserPassAndExtraData(Calculator& theCommands, const Expression& input, List<std::string>& outputData);
  bool ComputeAndStoreActivationToken(std::stringstream* commentsOnFailure, DatabaseRoutines& theRoutines);
  void ComputeShaonedSaltedPassword();
  bool GetActivationAbsoluteAddress
  (std::string& output, DatabaseRoutines& theRoutines, std::stringstream& comments)
  ;
  bool GetActivationAddress
  (std::string& output, const std::string& calculatorBase, DatabaseRoutines& theRoutines,
   std::stringstream& comments)
  ;
  static std::string GetActivationAddressFromActivationToken
  (const std::string& theActivationToken, const std::string& calculatorBase,
   const std::string& inputUserNameUnsafe, const std::string& inputEmailUnsafe
   )
  ;
  bool ComputeAndStoreActivationEmailAndTokens
  (std::stringstream* commentsInFailure, std::stringstream* commentsGeneral, DatabaseRoutines& theRoutines);
  bool SendActivationEmail(DatabaseRoutines& theRoutines, std::stringstream& comments);
  std::string ToString();
  std::string ToStringSelectedColumns();
  UserCalculator();
  ~UserCalculator();
};

class DatabaseRoutines
{
public:
  std::string databasePassword;
  std::string hostname;
  std::string databaseUser;
  std::string theDatabaseName;
  int MaxNumRowsToFetch;
  bool flagFirstLogin;
  MYSQL *connection; // Create a pointer to the MySQL instance
  operator bool()const
  { return false;
  }
  static std::string GetTableUnsafeNameUsersOfFile(const std::string& inputFileName);
  bool startMySQLDatabaseIfNotAlreadyStarted(std::stringstream* commentsOnFailure);
  bool startMySQLDatabase(std::stringstream* commentsOnFailure, bool* outputfirstLogin);
  static std::string ToStringSuggestionsReasonsForFailure
(const std::string& inputUsernameUnsafe, DatabaseRoutines& theRoutines, UserCalculator& theUser)
;
  bool InsertRow
  (const std::string& primaryKeyUnsafe, const std::string& primaryValueUnsafe,
   const std::string& tableNameUnsafe, std::stringstream* commentsOnFailure)
  ;
  bool ColumnExists(const std::string& columnNameUnsafe, const std::string& tableNameUnsafe, std::stringstream& commentsStream);
  bool CreateColumn(const std::string& columnNameNameUnsafe, const std::string& tableNameUnsafe,
                    std::stringstream& commentsOnCreation);
  bool TableExists(const std::string& tableNameUnsafe, std::stringstream* commentsOnFailure);
  bool RowExists
  (const MySQLdata& key, const MySQLdata& value, const MySQLdata& tableName, std::stringstream* comments)
   ;
  bool FetchTableNames
(List<std::string>& output, std::stringstream& comments)
;
  bool FetchTableFromDatabaseIdentifier
(List<List<std::string> >& output,
 List<std::string>& outputColumnLabels,
 bool& outputWasTruncated, int& actualNumRowsIfTruncated,
 const std::string& tableIdentifier, std::stringstream& comments)
;
  bool ReadProblemDatabaseInfo
  (const std::string& problemHomeName, std::string& outputString,
   std::stringstream& commentsOnFailure)
  ;
  bool StoreProblemDatabaseInfo
  (const UserCalculatorData& theUser, std::stringstream& commentsOnFailure)
  ;
  bool FetchEntry
  (const MySQLdata& key, const MySQLdata& valueSearchKey, const MySQLdata& tableName,
   const MySQLdata& desiredColumn, std::string& outputUnsafe, std::stringstream* failureComments)
  ;
    bool SetEntry
  (const MySQLdata& key, const MySQLdata& keyValue, const MySQLdata& table,
   const MySQLdata& columnToSet, const MySQLdata& valueToSet,
   std::stringstream* failureComments)
   ;
bool PrepareClassData
(const std::string& classFileName, List<List<std::string> >& outputUserTable,
 List<std::string>& outputLabelsUserTable,
  std::stringstream& commentsOnFailure)
  ;
bool FetchAllUsers
(List<List<std::string> >& outputUserTable, List<std::string>& outputLabelsUserTable,
  std::stringstream& commentsOnFailure)
  ;
  bool FetchTablE
(List<List<std::string> >& output,
 List<std::string>& outputColumnLabels,
 bool& outputWasTruncated, int& actualNumRowsIfTruncated,
 const MySQLdata& inputTable, std::stringstream& comments)
  ;
  bool AddUsersFromEmails
  (const std::string& emailList, const std::string& userPasswords, std::string& userRole, std::string& userGroup,
 std::stringstream& comments, int& outputNumNewUsers, int& outputNumUpdatedUsers)

  ;
  bool SendActivationEmail
  (const std::string& emailList,
   std::stringstream* commentsOnFailure,
   std::stringstream* commentsGeneral,
   std::stringstream* commentsGeneralSensitive
  );
  bool SendActivationEmail
  (const List<std::string>& theEmails,
   std::stringstream* commentsOnFailure,
   std::stringstream* commentsGeneral,
   std::stringstream* commentsGeneralSensitive
  );

  std::string ToString();
  std::string ToStringAllUsersHTMLFormat();
  std::string ToStringOneEntry();
  std::string ToStringCurrentTableHTML();
  std::string ToStringAllTables();
  std::string ToStringTablE(const MySQLdata& inputTable);
  std::string ToStringTableFromTableIdentifier(const std::string& tableIdentifier);

  bool CreateTable
  (const std::string& tableNameUnsafe, const std::string& desiredTableContent,
   std::stringstream* commentsOnCreation, bool* outputTableNewlyCreated);

  static bool innerTestDatabase(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserPassword(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSendActivationEmailUsers(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddUsersFromEmailListAndCourseName(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDeleteUser(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetUserPassword(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetAuthentication(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDisplayTables(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDisplayDatabaseTable(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerUserExists(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserDBEntry(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserDetails(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddStudentToClass(Calculator& theCommands, const Expression& input, Expression& output);

  DatabaseRoutines();
  ~DatabaseRoutines();
};

struct DatabaseQuery
{
public:
  DatabaseRoutines* parent;
  MYSQL_RES* theQueryResult;
  MYSQL_ROW currentRow;
  int MaxNumRowsToRead;
  int numRowsRead;
  bool flagOutputWasTruncated;
  std::string theQueryString;
  std::string firstResultString;
  List<List<std::string> > allQueryResultStrings;
  List<List<List<std::string> > > additionalResultStrings;
  bool flagQuerySucceeded;
  bool flagQueryReturnedResult;
  void close();
  DatabaseQuery(DatabaseRoutines& inputParent, const std::string& inputQuery, std::stringstream* outputFailureComments=0, int inputMaxNumRowsToRead=1000);
  ~DatabaseQuery();
};


#endif // MACRO_use_MySQL
#endif // vpfHeader7_databaseMySQL_already_included
