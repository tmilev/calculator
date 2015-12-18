//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader7_databaseMySQL_already_included
#define vpfHeader7_databaseMySQL_already_included
#ifdef MACRO_use_MySQL
#include "vpfHeader3Calculator0_Interface.h"
#include <mysql/mysql.h>
#include <ctime>

//Command for installation of mysql on Ubuntu:
//sudo apt-get install libmysqlclient-dev
static ProjectInformationInstance ProjectInfoVpf8_1HeaderDatabaseInterface_MySQLx(__FILE__, "MySQL interface header. ");

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
  TimeWrapper();
};

class DatabaseRoutines;
class UserCalculator
{
  public:
  std::string usernamePlusPassWord;
  std::string username;
  std::string password;
  std::string email;
  List<std::string> selectedColumns;
  List<std::string> selectedColumnValues;
  List<std::string> selectedColumnValuesAvailable;
  std::string authenticationToken;
  TimeWrapper authenticationTokenCreationTime;
  bool FetchOneColumn(std::string& columnName, std::string& output, DatabaseRoutines& theRoutines);
  void FetchColumns(DatabaseRoutines& theRoutines);
  bool FetchAuthenticationTokenCreationTime(DatabaseRoutines& theRoutines);
  bool AuthenticateWithUserNameAndPass(DatabaseRoutines& theRoutines);
  bool AuthenticateWithToken(DatabaseRoutines& theRoutines);
  bool Authenticate(DatabaseRoutines& theRoutines);
  bool ComputeAuthenticationToken(DatabaseRoutines& theRoutines);
  std::string GetPassword(DatabaseRoutines& theRoutines);
  bool SetPassword(DatabaseRoutines& theRoutines);
  bool TryToLogIn(DatabaseRoutines& theRoutines);
  bool DeleteMe(DatabaseRoutines& theRoutines);
  bool Iexist(DatabaseRoutines& theRoutines);
  bool CreateMeIfUsernameUnique(DatabaseRoutines& theRoutines);
  bool getUserPassAndEmail(Calculator& theCommands, const Expression& input);
  bool getUserAndPass(Calculator& theCommands, const Expression& input);
  bool getUser(Calculator& theCommands, const Expression& input);
  bool getUserPassAndSelectedColumns(Calculator& theCommands, const Expression& input);
  std::string ToString();
  std::string ToStringSelectedColumns();
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
  std::string ToString();
  static bool innerTestDatabase(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestLogin(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserPassword(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddUser(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDeleteUser(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetUserPassword(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetAuthentication(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserDBEntry(Calculator& theCommands, const Expression& input, Expression& output);
  DatabaseRoutines();
  ~DatabaseRoutines();
};

struct DatabaseQuery
{
public:
  DatabaseRoutines* parent;
  MYSQL_RES* theQueryResult;
  MYSQL_ROW currentRow;
  std::string theQueryString;
  std::string theQueryResultString;
  bool flagQuerySucceeded;
  bool flagQueryReturnedResult;
  DatabaseQuery(DatabaseRoutines& inputParent, const std::string& inputQuery);
  ~DatabaseQuery();
};


#endif // MACRO_use_MySQL
#endif // vpfHeader7_databaseMySQL_already_included
