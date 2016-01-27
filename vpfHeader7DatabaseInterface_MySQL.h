//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader7_databaseMySQL_already_included
#define vpfHeader7_databaseMySQL_already_included
#include "vpfHeader3Calculator0_Interface.h"
static ProjectInformationInstance ProjectInfoVpf8_1HeaderDatabaseInterface_MySQLx(__FILE__, "MySQL interface header. ");

class DatabaseRoutinesGlobalFunctions
{
public:
  static bool LogoutViaDatabase();
  static bool LoginViaDatabase(const std::string& inputUsername, const std::string& inputPassword, std::string& inputOutputAuthenticationToken, std::stringstream* comments);
  static bool SetPassword
  (const std::string& inputUsername, const std::string& inputNewPassword,
   std::stringstream& comments);
  static bool SetEntry
  (const std::string& inputUsername, const std::string& tableName, const std::string& keyName,
   const std::string& value, std::stringstream& comments);
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

#ifdef MACRO_use_MySQL
#include <mysql/mysql.h>
#include <ctime>

//Command for installation of mysql on Ubuntu:
//sudo apt-get install libmysqlclient-dev

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

struct TimeWrapper
{
public:
  tm theTime;
  std::string theTimeStringNonReadable;
  void AssignLocalTime();
  void ComputeTimeStringNonReadable();
  std::string ToStringHumanReadable();
  void operator=(const std::string& inputTime);
  double SubtractAnotherTimeFromMeAndGet_APPROXIMATE_ResultInHours(const TimeWrapper& other);
  TimeWrapper();
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
private:
  std::string usernameSafe;
  std::string emailSafe;
  std::string currentTableSafe;
  std::string currentTableUnsafe;
  std::string activationTokenSafe;
public:
  double approximateHoursSinceLastTokenWasIssued;
  std::string usernamePlusPassWord;
  std::string usernameUnsafe;
  std::string enteredPassword;
  std::string actualShaonedSaltedPassword;
  std::string enteredShaonedSaltedPassword;
  std::string emailUnsafe;
  std::string activationTokenUnsafe;
  List<std::string> selectedColumnsUnsafe;
  List<std::string> selectedColumnValuesUnsafe;
  List<std::string> selectedColumnsRetrievalFailureRemarks;

  List<std::string> selectedRowFieldsUnsafe;
  List<std::string> selectedRowFieldNamesUnsafe;
  std::string enteredAuthenticationTokenUnsafe;
  std::string actualAuthenticationTokeNUnsafe;
  TimeWrapper authenticationTokenCreationTime;
  bool flagNewAuthenticationTokenComputedUserNeedsIt;
  bool SetCurrentTable(const std::string& inputTableNameUnsafe);
  std::string GetSelectedRowEntry(const std::string& theKey);
  bool FetchOneUserRow
  (DatabaseRoutines& theRoutines, std::stringstream& failureStream);
  bool FetchOneColumn
  (const std::string& columnNameUnsafe, std::string& outputUnsafe,
   DatabaseRoutines& theRoutines, bool recomputeSafeEntries, std::stringstream* failureComments=0);
  void FetchColumns(DatabaseRoutines& theRoutines, bool recomputeSafeEntries);
  bool AuthenticateWithUserNameAndPass(DatabaseRoutines& theRoutines, bool recomputeSafeEntries);
  bool AuthenticateWithToken(DatabaseRoutines& theRoutines, bool recomputeSafeEntries);
  bool Authenticate(DatabaseRoutines& theRoutines, bool recomputeSafeEntries);
  bool ResetAuthenticationToken(DatabaseRoutines& theRoutines, bool recomputeSafeEntries);
  std::string GetPassword(DatabaseRoutines& theRoutines);
  bool SetColumnEntry
  (const std::string& columnNameUnsafe, const std::string& theValueUnsafe,
   DatabaseRoutines& theRoutines, bool recomputeSafeEntries, std::stringstream* failureComments=0);
  bool SetPassword(DatabaseRoutines& theRoutines, bool recomputeSafeEntries, std::stringstream& commentsOnFailure);
  bool TryToLogIn(DatabaseRoutines& theRoutines);
  bool DeleteMe(DatabaseRoutines& theRoutines, bool recomputeSafeEntries);
  bool Iexist(DatabaseRoutines& theRoutines, bool recomputeSafeEntries);
  bool CreateMeIfUsernameUnique(DatabaseRoutines& theRoutines, bool recomputeSafeEntries);
  bool ComputeSafeObjectNames();
  static bool IsAcceptableDatabaseInpuT(const std::string& input, std::stringstream* comments);
  static bool IsAcceptableCharDatabaseInpuT(char theChar);
  bool getUserPassAndEmail(Calculator& theCommands, const Expression& input);
  bool getUserAndPass(Calculator& theCommands, const Expression& input);
  bool getUser(Calculator& theCommands, const Expression& input);
  bool getUserPassAndSelectedColumns(Calculator& theCommands, const Expression& input);
  bool getUserPassAndExtraData(Calculator& theCommands, const Expression& input, List<std::string>& outputData);
  void ComputeShaonedSaltedPassword(bool recomputeSafeEntries);
  bool SendActivationEmail(DatabaseRoutines& theRoutines, std::stringstream& comments);
  std::string ToString();
  std::string ToStringSelectedColumns();
  UserCalculator();
  ~UserCalculator();
};

class DatabaseRoutines
{
  template<typename anyType>
  friend DatabaseRoutines& operator << (DatabaseRoutines& output, const anyType& any)
  { output.comments << any;
    return output;
  }
public:
  std::string databasePassword;
  std::string databaseUser;
  std::string theDatabaseName;
  std::string hostname;
  std::stringstream comments;
  int MaxNumRowsToFetch;
  MYSQL *connection; // Create a pointer to the MySQL instance
  operator bool()const
  { return false;
  }
  bool startMySQLDatabaseIfNotAlreadyStarted();
  bool startMySQLDatabase();

  bool ColumnExists(const std::string& columnNameUnsafe, const std::string& tableNameUnsafe, std::stringstream& commentsStream);
  bool CreateColumn(const std::string& columnNameNameUnsafe, const std::string& tableNameUnsafe,
                    std::stringstream& commentsOnCreation);
  bool TableExists(const std::string& tableNameUnsafe);
  bool FetchTableNames
(List<std::string>& output, std::stringstream& comments)
;
  bool FetchTable
(List<List<std::string> >& output,
 List<std::string>& outputColumnLabels,
 bool& outputWasTruncated, int& actualNumRowsIfTruncated,
 const std::string& tableNameUnsafe, std::stringstream& comments)

  ;
  bool AddUsersFromEmailsAndCourseName(const std::string& emailList, const std::string& ExamHomeFile, std::stringstream& comments);
  bool AddUsersFromEmails(const std::string& emailList, std::stringstream& comments);
  bool SendActivationEmail(const std::string& emailList, std::stringstream& comments);
  bool SendActivationEmail(const List<std::string>& theEmails, bool forceResend, std::stringstream& comments);
  bool ExtractEmailList(const std::string& emailList, List<std::string>& outputList, std::stringstream& comments);

  std::string ToString();
  std::string ToStringAllUsersHTMLFormat();
  std::string ToStringCurrentTableHTML();
  std::string ToStringAllTables();
  std::string ToStringTable(const std::string& inputTableNameUnsafe);

  bool CreateTable
  (const std::string& tableNameUnsafe, const std::string& desiredTableContent,
   std::stringstream* commentsOnCreation=0);

  static bool innerTestDatabase(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestLogin(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserPassword(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddUser(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddUsersFromEmailList(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSendActivationEmailUsers(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddUsersFromEmailListAndCourseName(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDeleteUser(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetUserPassword(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetAuthentication(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDisplayTables(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDisplayDatabaseTable(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserDBEntry(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserDetails(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerCreateTeachingClass(Calculator& theCommands, const Expression& input, Expression& output);
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
  std::stringstream* failurecomments;
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
