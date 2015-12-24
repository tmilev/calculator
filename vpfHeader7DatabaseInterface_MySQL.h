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
  public:
  double approximateHoursSinceLastTokenWasIssued;
  std::string usernamePlusPassWord;
  std::string username;
  std::string enteredPassword;
  std::string actualShaonedSaltedPassword;
  std::string enteredShaonedSaltedPassword;
  std::string email;
  List<std::string> selectedColumns;
  List<std::string> selectedColumnValues;
  List<std::string> selectedColumnsRetrievalFailureRemarks;
  std::string enteredAuthenticationToken;
  std::string actualAuthenticationToken;
  TimeWrapper authenticationTokenCreationTime;
  bool flagNewAuthenticationTokenComputedUserNeedsIt;
  bool FetchOneColumn
  (const std::string& columnName, std::string& output, DatabaseRoutines& theRoutines, std::stringstream* failureComments=0);
  void FetchColumns(DatabaseRoutines& theRoutines);
  bool AuthenticateWithUserNameAndPass(DatabaseRoutines& theRoutines);
  bool AuthenticateWithToken(DatabaseRoutines& theRoutines);
  bool Authenticate(DatabaseRoutines& theRoutines);
  bool ResetAuthenticationToken(DatabaseRoutines& theRoutines);
  std::string GetPassword(DatabaseRoutines& theRoutines);
  bool SetColumnEntry(const std::string& columnName, const std::string& theValue, DatabaseRoutines& theRoutines, std::stringstream* failureComments=0);
  bool SetPassword(DatabaseRoutines& theRoutines);
  bool TryToLogIn(DatabaseRoutines& theRoutines);
  bool DeleteMe(DatabaseRoutines& theRoutines);
  bool Iexist(DatabaseRoutines& theRoutines);
  bool CreateMeIfUsernameUnique(DatabaseRoutines& theRoutines);
  bool UserEntriesAreValidObjectNames(std::stringstream* comments);
  static bool IsAcceptableObjectName(const std::string& input, std::stringstream* comments);
  bool getUserPassAndEmail(Calculator& theCommands, const Expression& input);
  bool getUserAndPass(Calculator& theCommands, const Expression& input);
  bool getUser(Calculator& theCommands, const Expression& input);
  bool getUserPassAndSelectedColumns(Calculator& theCommands, const Expression& input);
  bool getUserPassAndExtraData(Calculator& theCommands, const Expression& input, List<std::string>& outputData);
  void ComputeShaonedSaltedPassword();
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
  bool TableExists(const std::string& tableName);
  std::string ToString();
  std::string ToStringAllUsersHTMLFormat();
  bool CreateTable
(const std::string& tableName, const std::string& desiredTableContent);

  static bool innerTestDatabase(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestLogin(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserPassword(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddUser(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDeleteUser(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetUserPassword(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetAuthentication(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserDBEntry(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserDetails(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddTeachingClass(Calculator& theCommands, const Expression& input, Expression& output);
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
