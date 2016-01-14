//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader7_databaseMySQL_already_included
#define vpfHeader7_databaseMySQL_already_included
#include "vpfHeader3Calculator0_Interface.h"
static ProjectInformationInstance ProjectInfoVpf8_1HeaderDatabaseInterface_MySQLx(__FILE__, "MySQL interface header. ");

class DatabaseRoutinesGlobalFunctions
{
public:
  static bool LoginViaDatabase(const std::string& inputUsername, const std::string& inputPassword, std::string& inputOutputAuthenticationToken);
  static bool SetEntry
  (const std::string& inputUsername, const std::string& tableName, const std::string& keyName,
   const std::string& value, std::stringstream& comments);
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
public:
  double approximateHoursSinceLastTokenWasIssued;
  std::string usernamePlusPassWord;
  std::string usernameUnsafe;
  std::string enteredPassword;
  std::string actualShaonedSaltedPassword;
  std::string enteredShaonedSaltedPassword;
  std::string emailUnsafe;
  List<std::string> selectedColumnsUnsafe;
  List<std::string> selectedColumnValuesUnsafe;
  List<std::string> selectedColumnsRetrievalFailureRemarks;
  std::string enteredAuthenticationTokenUnsafe;
  std::string actualAuthenticationTokeNUnsafe;
  TimeWrapper authenticationTokenCreationTime;
  bool flagNewAuthenticationTokenComputedUserNeedsIt;
  bool SetCurrentTable(const std::string& inputTableNameUnsafe);
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
  bool SetPassword(DatabaseRoutines& theRoutines, bool recomputeSafeEntries);
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
  MYSQL *connection; // Create a pointer to the MySQL instance
  operator bool()const
  { return false;
  }
  bool startMySQLDatabase();
  bool TableExists(const std::string& tableNameUnsafe);
  std::string ToString();
  std::string ToStringAllUsersHTMLFormat();
  bool CreateTable
(const std::string& tableNameUnsafe, const std::string& desiredTableContent, std::stringstream* commentsOnCreation=0);

  static bool innerTestDatabase(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestLogin(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserPassword(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddUser(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDeleteUser(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetUserPassword(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetAuthentication(Calculator& theCommands, const Expression& input, Expression& output);
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
  std::stringstream* failurecomments;
  std::string theQueryString;
  std::string firstResultString;
  List<List<std::string> > allQueryResultStrings;
  bool flagQuerySucceeded;
  bool flagQueryReturnedResult;
  DatabaseQuery(DatabaseRoutines& inputParent, const std::string& inputQuery, std::stringstream* outputFailureComments=0);
  ~DatabaseQuery();
};


#endif // MACRO_use_MySQL
#endif // vpfHeader7_databaseMySQL_already_included
