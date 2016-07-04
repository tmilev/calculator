//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader7_databaseMySQL_already_included
#define vpfHeader7_databaseMySQL_already_included
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader1General5TimeDate.h"

static ProjectInformationInstance ProjectInfoVpf8_1HeaderDatabaseInterface_MySQLx(__FILE__, "MySQL interface header. ");

class DatabaseRoutinesGlobalFunctions
{
public:
  static bool LogoutViaDatabase();
  static bool LoginViaDatabase
(const std::string& inputUsernameUnsafe, const std::string& inputPassword,
 std::string& inputOutputAuthenticationToken, std::string& outputUserRole, std::stringstream* comments)

  ;
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

struct Answer
{
  bool flagAutoGenerateSubmitButtons;
  bool flagAutoGenerateMQButtonPanel;
  bool flagAutoGenerateMQfield;
  bool flagAutoGenerateVerificationField;
  int numSubmissions;
  int numCorrectSubmissions;
  std::string answerId;
  std::string submitButtonIds;
  std::string interpretButtonIds;
  std::string answerIdVariableName;
  std::string verificationSpanId;
  std::string previewAnswerJavascript;
  std::string htmlMQjavascript;
  std::string htmlMQfield;
  std::string htmlContainerMQButtonPanel;
  std::string htmlButtonInterpret;
  std::string htmlButtonAnswer;
  std::string htmlButtonSubmit;
  std::string htmlTextareaLatexAnswer;
  std::string htmlSpanVerifyAnswer;
  //////////////////////////////////////
  std::string MQButtonPanelId;
  std::string MQfieldVariable;
  std::string MQobject;
  std::string MQfieldId;
  std::string MQUpdateFunction;
  std::string firstCorrectAnswer;
  Answer()
  { this->numSubmissions=0;
    this->numCorrectSubmissions=0;
    this->flagAutoGenerateSubmitButtons=true;
    this->flagAutoGenerateMQButtonPanel=true;
    this->flagAutoGenerateMQfield=true;
    this->flagAutoGenerateVerificationField=true;
  }
};

struct ProblemData
{
public:
  bool flagRandomSeedComputed;
  unsigned int randomSeed;
  std::string ProblemWeightUserInput;
  bool flagProblemWeightIsOK;
  Rational ProblemWeight;
  Rational Points;
  int numCorrectlyAnswered;
  int totalNumSubmissions;
//  int numAnswersSought;
  List<Answer> theAnswers;
  List<std::string> inputNonAnswerIds;
  List<List<std::string> > commandsForPreview;
  List<std::string> commandsForGiveUpAnswer;
  List<List<std::string> > commentsBeforeSubmission;
  List<List<std::string> > commentsAfterSubmission;
  void AddEmptyAnswerIdOnTop(const std::string& inputAnswerId);
  int GetAnswerIdIndex(const std::string& inputAnswerId);
//  List<List<std::string> > allAnswers;
  ProblemData();
  bool LoadFrom(const std::string& inputData, std::stringstream& commentsOnFailure);
  std::string Store();
  std::string ToString();
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

  EmailRoutines();
  std::string GetCommandToSendEmailWithMailX();
};

class MySQLdata{
//This class is needed to attempt to deal with mySQL's
//numerous design errors, to the extent possible.
//Documenting those errors for the interested reader.
//1. Mysql identifiers have max length of 64 characters.
//   Workaround this MySQL bug: when used as identifiers, strings are
//   trimmed. We use the first 30 characters
//   + we append SHA-1 of the entire string.
//   Motivation: we don't loose human-readability for small strings.
//2. Mysql identifiers cannot have ` characters in them.
//   Workaround this MySQL bug: we url-encode any data stored in
//   the database.
//   Motivation: we retain limited human-readability.
public:
  std::string value;
  MySQLdata(const std::string& other)
  { this->value=other;
  }
  MySQLdata(){}
  bool operator==(const std::string& other)
  { return this->value==other;
  }
  bool operator!=(const std::string& other)
  { return !(*this==other);
  }
  void operator=(const std::string& other)
  { this->value=other;
  }
  std::string GetDatA()const;
  std::string GetIdentifierNoQuotes()const;
  std::string GetIdentifieR()const;
};

class DatabaseRoutines;
class UserCalculator
{
// Unsafe entries may contain arbitrary strings.
// Safe entries, when enclosed with "" in ANSI mode are guaranteed to be valid safe Database identifiers.
// In other words, safe entries are guaranteed to not allow bobby-tables-exploits-of-a-mom (https://xkcd.com/327/).
// Users of this code should not touch any safe entries directly.
// Instead users should only modify the unsafe entries.
// Those are internally (and automatically) converted to safe entries (stored in the private variables below), and only then stored in
// the database.
public:
  double approximateHoursSinceLastTokenWasIssued;
  std::string usernamePlusPassWord;
  MySQLdata username;
  MySQLdata email;
  MySQLdata currentTable;
  MySQLdata activationToken;
  MySQLdata enteredAuthenticationToken;
  MySQLdata actualAuthenticationToken;
  std::string extraInfoUnsafe;
  std::string enteredPassword;
  std::string actualShaonedSaltedPassword;
  std::string enteredShaonedSaltedPassword;
  std::string userRole;
  List<std::string> selectedColumnsUnsafe;
  List<std::string> selectedColumnValuesUnsafe;
  List<std::string> selectedColumnsRetrievalFailureRemarks;

  List<std::string> selectedRowFieldsUnsafe;
  List<std::string> selectedRowFieldNamesUnsafe;

  HashedList<std::string, MathRoutines::hashString> problemNames;
  List<ProblemData> problemData;
  TimeWrapper authenticationCreationTime;
  Rational pointsEarned;
  Rational pointsMax;

  void ComputePointsEarned
  (const HashedList<std::string, MathRoutines::hashString>& gradableProblems,
   const List<std::string>& problemWeights);
  ProblemData& HasProblemData(const std::string& problemName);
  ProblemData& GetProblemDataAddIfNotPresent(const std::string& problemName);
  void SetProblemData(const std::string& problemName, const ProblemData& inputData);
  bool flagNewAuthenticationTokenComputedUserNeedsIt;
  bool LoadProblemStringFromDatabase(DatabaseRoutines& theRoutines, std::string& output, std::stringstream& commentsOnFailure);
  bool InterpretDatabaseProblemData(const std::string& theInfo, std::stringstream& commentsOnFailure);
  bool StoreProblemDataToDatabase(DatabaseRoutines& theRoutines, std::stringstream& commentsOnFailure);
  std::string GetSelectedRowEntry(const std::string& theKey);
  bool FetchOneUserRow
  (DatabaseRoutines& theRoutines, std::stringstream& failureStream);
  bool FetchOneColumn
  (const std::string& columnNameUnsafe, std::string& outputUnsafe,
   DatabaseRoutines& theRoutines, std::stringstream* failureComments=0);
  void FetchColumns(DatabaseRoutines& theRoutines);
  bool AuthenticateWithUserNameAndPass(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure);
  bool AuthenticateWithToken(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure);
  bool Authenticate(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure);
  bool ResetAuthenticationToken(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure);
  std::string GetPassword(DatabaseRoutines& theRoutines);
  bool SetColumnEntry
  (const std::string& columnNameUnsafe, const std::string& theValueUnsafe,
   DatabaseRoutines& theRoutines, std::stringstream* failureComments=0);
  bool SetPassword(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure);
  bool DeleteMe(DatabaseRoutines& theRoutines, std::stringstream& commentsOnFailure);
  bool Iexist(DatabaseRoutines& theRoutines);
  bool IamPresentInTable(DatabaseRoutines& theRoutines, const std::string& tableNameUnsafe);
  bool CreateMeIfUsernameUnique(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure);
  static bool IsAcceptableDatabaseInpuT(const std::string& input, std::stringstream* comments);
  static bool IsAcceptableCharDatabaseInpuT(char theChar);
  bool getUserPassAndEmail(Calculator& theCommands, const Expression& input);
  bool getUserAndPass(Calculator& theCommands, const Expression& input);
  bool getUser(Calculator& theCommands, const Expression& input);
  bool getUserPassAndSelectedColumns(Calculator& theCommands, const Expression& input);
  bool getUserPassAndExtraData(Calculator& theCommands, const Expression& input, List<std::string>& outputData);
  void ComputeActivationToken();
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
   const std::string& inputUserNameUnsafe)
  ;
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
  std::string databaseUser;
  std::string theDatabaseName;
  std::string hostname;
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
   const std::string& tableNameUnsafe, std::stringstream& commentsOnFailure)
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
  (const std::string& problemHomeName, const std::string& inputString,
   std::stringstream& commentsOnFailure)
  ;
  bool MergeProblemInfoInDatabase
  (const std::string& problemHomeName, std::string& inputString,
   std::stringstream& commentsOnFailure)
  ;
  bool ReadProblemInfo
  (const std::string& stringToReadFrom, HashedList<std::string, MathRoutines::hashString>& outputProblemNames,
   List<std::string>& outputWeights,
   List<List<std::string> >& outputSections, List<List<std::string> >& outputDeadlinesPerSection,
   std::stringstream& commentsOnFailure)
      ;
  void StoreProblemInfo
  (std::string& outputString, const HashedList<std::string, MathRoutines::hashString>& inputProblemNames,
   const List<std::string>& inputWeights, const List<List<std::string> >& inputSections,
   const List<List<std::string> >& inputDeadlines)
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
  std::string ToStringClassDetails
(bool adminsOnly, List<List<std::string> >& userTable, List<std::string>& userLabels,
 HashedList<std::string, MathRoutines::hashString>& databaseSpanList,
 List<std::string>& databaseProblemWeights
 )
  ;
  bool FetchTablE
(List<List<std::string> >& output,
 List<std::string>& outputColumnLabels,
 bool& outputWasTruncated, int& actualNumRowsIfTruncated,
 const MySQLdata& inputTable, std::stringstream& comments)
  ;
  bool AddUsersFromEmailsAndCourseName
  (const std::string& emailList, const std::string& ExamHomeFile, const std::string& extraInfo,
   std::stringstream& comments)
   ;
  bool AddUsersFromEmails
  (bool doSendEmails, const std::string& emailList, const std::string& extraInfo, bool& outputSentAllEmails,
   std::stringstream& comments)
  ;
  bool SendActivationEmail(const std::string& emailList, std::stringstream& comments);
  bool SendActivationEmail(const List<std::string>& theEmails, std::stringstream& comments);
  bool ExtractEmailList(const std::string& emailList, List<std::string>& outputList, std::stringstream& comments);

  std::string ToString();
  std::string ToStringAllUsersHTMLFormat();
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
  bool flagQuerySucceeded;
  bool flagQueryReturnedResult;
  void close();
  DatabaseQuery(DatabaseRoutines& inputParent, const std::string& inputQuery, std::stringstream* outputFailureComments=0, int inputMaxNumRowsToRead=1000);
  ~DatabaseQuery();
};


#endif // MACRO_use_MySQL
#endif // vpfHeader7_databaseMySQL_already_included
