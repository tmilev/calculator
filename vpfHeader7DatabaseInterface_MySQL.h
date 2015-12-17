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
  std::string timeString;
  void AssignLocalTime();
  void ComputeTimeString();
  std::string ToStringHumanReadable();
  void operator=(const std::string& inputTime);
  TimeWrapper();
};

class DatabaseRoutines
{
  template<typename anyType>
  friend DatabaseRoutines& operator << (DatabaseRoutines& output, const anyType& any)
  { output.comments << any;
    return output;
  }
public:
  std::string usernamePlusPassWord;
  std::string username;
  std::string password;
  std::string email;
  std::string theDatabaseName;
  std::string hostname;
  std::string authenticationToken;
  TimeWrapper authenticationTokenCreationTime;
  std::stringstream comments;
  MYSQL *connection; // Create a pointer to the MySQL instance
  operator bool()const
  { return false;
  }
  bool startMySQLDatabase();
  std::string ToString();
  bool TryToLogIn();
  std::string GetUserPassword();
  bool FoundUser();
  bool AddUserIfUserDoesNotExistAlready();
  bool SetUserPassword();
  bool ComputeAuthenticationToken();
  bool FetchAuthenticationTokenCreationTime();
  bool AuthenticateWithUserNameAndPass();
  bool AuthenticateWithToken();
  bool Authenticate();
  static bool innerTestDatabase(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTestLogin(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetUserPassword(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddUser(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetUserPassword(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetAuthentication(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerGetAuthenticationTokenCreationTime(Calculator& theCommands, const Expression& input, Expression& output);
  bool getUserPassAndEmail(Calculator& theCommands, const Expression& input);
  bool getUserAndPass(Calculator& theCommands, const Expression& input);
  bool getUser(Calculator& theCommands, const Expression& input);
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
