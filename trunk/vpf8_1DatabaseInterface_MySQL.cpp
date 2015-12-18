//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifdef MACRO_use_MySQL
#include "vpfHeader7DatabaseInterface_MySQL.h"
#include "vpfHeader5Crypto.h"
#include <time.h>
#include <ctime>
ProjectInformationInstance ProjectInfoVpf8_1MySQLcpp(__FILE__, "MySQL interface. ");

std::string UserCalculator::ToStringSelectedColumns()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringSelectedColumns");
  std::stringstream out;
  out << this->selectedColumns.size << " columns selected. ";
  for (int i=0; i<this->selectedColumns.size; i++)
    out << "<br>" << this->selectedColumns[i] << ": " << this->selectedColumnValues[i] << this->selectedColumnValuesAvailable[i];
  return out.str();
}


std::string DatabaseRoutines::ToString()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToString");
  std::stringstream out;
  out << "Database name: " << this->theDatabaseName;
  return out.str();

}

std::string UserCalculator::ToString()
{ MacroRegisterFunctionWithName("UserCalculator::ToString");
  std::stringstream out;
  out << "Calculator user: " << this->username;
  return out.str();
}

UserCalculator::~UserCalculator()
{ for (unsigned i=0; i<this->password.size(); i++)
    this->password[i]=' ';
  for (unsigned i=0; i<this->usernamePlusPassWord.size(); i++)
    this->usernamePlusPassWord[i]=' ';
  for (unsigned i=0; i<this->authenticationToken.size(); i++)
    this->authenticationToken[i]=' ';
}

DatabaseRoutines::DatabaseRoutines()
{ this->connection=0;
}

DatabaseRoutines::~DatabaseRoutines()
{ if (this->connection!=0)
    mysql_close(this->connection);   // Close and shutdown
  this->connection=0;
}

bool UserCalculator::TryToLogIn(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::TryToLogIn");
  if (!theRoutines.startMySQLDatabase())
    return false;
  DatabaseQuery theQuery(theRoutines, "SELECT " +this->username + " FROM users LIMIT 1");
  if (!theQuery.flagQueryReturnedResult)
    return false;
  return false;
}

DatabaseQuery::DatabaseQuery(DatabaseRoutines& inputParent, const std::string& inputQuery)
{ MacroRegisterFunctionWithName("DatabaseQuery::DatabaseQuery");
  this->theQueryString=inputQuery;
  this->parent=&inputParent;
  this->flagQuerySucceeded=false;
  this->flagQueryReturnedResult=false;
  this->theQueryResult=0;
  if (this->parent->connection==0)
    if (!this->parent->startMySQLDatabase())
      return;
  int queryError=mysql_query(this->parent->connection, this->theQueryString.c_str());
  if (queryError!=0)
  { this->parent->comments << "Query<br>" << this->theQueryString << "<br>failed. ";
    this->parent->comments << mysql_error(this->parent->connection);
    return;
  }
  this->flagQuerySucceeded=true;
  this->parent->comments << "Query succeeded. ";
  this->theQueryResult= mysql_store_result(this->parent->connection);
  if (this->theQueryResult==0)
  { this->parent->comments << "mysql_store_result failed. ";
    return;
  }
  this->currentRow=mysql_fetch_row(this->theQueryResult);
  if (this->currentRow==0)
  { this->parent->comments << "No rows returned by the query. ";
    return;
  }
  this->parent->comments << "Query " << this->theQueryString << " returned rows. ";
  this->flagQueryReturnedResult=true;
//  stOutput << "<br>the flag: " << this->flagQueryReturnedResult;
  std::stringstream theDataCivilizer;
//  stOutput << "<br>the flag: " << this->flagQueryReturnedResult;
  theDataCivilizer << *this->currentRow;
//  stOutput << "<br>the flag: " << this->flagQueryReturnedResult;
  this->theQueryResultString=theDataCivilizer.str();
//  stOutput << "<br>the flag: " << this->flagQueryReturnedResult;
}

DatabaseQuery::~DatabaseQuery()
{ //stOutput << "<br>DESTRUCTOR";
  if (this->theQueryResult!=0)
    mysql_free_result(this->theQueryResult);
  this->theQueryResult=0;
}

bool UserCalculator::FetchAuthenticationTokenCreationTime(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::FetchAuthenticationTokenCreationTime");
  DatabaseQuery theQuery
  (theRoutines, "SELECT authenticationTokenCreationTime FROM calculatorUsers.users WHERE user=\""+
   this->username+"\"");
  if (!theQuery.flagQueryReturnedResult)
    return false;
  this->authenticationTokenCreationTime=theQuery.theQueryResultString;
  return true;
}

bool UserCalculator::Authenticate(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  return true;
}

bool UserCalculator::AuthenticateWithUserNameAndPass(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  this->usernamePlusPassWord=this->username;
  this->usernamePlusPassWord+=this->password;
  //<-careful copying those around. We don't want to leave
  //any passwords in non-zeroed memory, even if properly freed.
  //Note the destructor of DatabaseRoutines zeroes some of the strings.
  DatabaseQuery theDBQuery(theRoutines, "SELECT password FROM calculatorUsers.users WHERE email=\"" + this->username + "\"");
  if (!theDBQuery.flagQueryReturnedResult)
    return false;
  if (theDBQuery.theQueryResultString!=Crypto::computeSha1outputBase64(this->usernamePlusPassWord))
    return theRoutines << "Database pass does not match your input which converts to: "
    << Crypto::computeSha1outputBase64(this->usernamePlusPassWord);
  return true;
}

bool UserCalculator::ComputeAuthenticationToken(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::ComputeAuthenticationToken");
  if (!this->AuthenticateWithUserNameAndPass(theRoutines))
    return false;
  //DatabaseQuery theQuery(*this, "SELECT authenticationTokenCreationTime FROM calculatorUsers.users WHERE user=\""+this->username + "\"");
  //if (theQuery.flagQueryReturnedResult)
  //  this->authentication="authenticationTokenCreationTime: "
  return true;
}

std::string UserCalculator::GetPassword(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::GetUserPassword");
  //stOutput << "Whats going on here<br>";
  DatabaseQuery theDBQuery(theRoutines, "SELECT password FROM calculatorUsers.users WHERE email=\"" + this->username + "\"");
  if (!theDBQuery.flagQueryReturnedResult)
    return theRoutines.comments.str();
  std::stringstream out;
  out << "password(sha-1): " << theDBQuery.theQueryResultString << "<br>comments: " << theRoutines.comments.str();
  return out.str();
}

bool UserCalculator::Iexist(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::Iexist");
  DatabaseQuery theQuery(theRoutines,
  "SELECT user FROM calculatorUsers.users where user='" + this->username + "' "
  );
  return theQuery.flagQueryReturnedResult;
}

bool UserCalculator::DeleteMe(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::DeleteMe");
  if (MathRoutines::StringBeginsWith(this->username, "deleted"))
    return theRoutines << "Usernames starting with 'deleted' cannot be deleted from the calculator interface. ";
  if (!this->Iexist(theRoutines))
    return theRoutines << "I was not able to find user: " << this->username << ". The user does not exist or there is something wrong with the database";
  TimeWrapper currentTime;
  currentTime.AssignLocalTime();
  currentTime.ComputeTimeStringNonReadable();
  std::string userArchiveName="deleted" + currentTime.theTimeStringNonReadable+ this->username;
  DatabaseQuery renamingQuery(theRoutines,
  "UPDATE users SET user='" + userArchiveName + "'"+
  "WHERE user='" + this->username +"'"
  );
  return renamingQuery.flagQuerySucceeded;
}

bool UserCalculator::CreateMeIfUsernameUnique(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::CreateMeIfUsernameUnique");
  if (this->Iexist(theRoutines))
    return theRoutines << "User " << this->username << " already exists. ";
  DatabaseQuery theQuery(theRoutines,
  "INSERT INTO calculatorUsers.users(user, email) VALUES('" + this->username + "', '"
  + this->email + "')"
  );
  return this->SetPassword(theRoutines);
}

bool UserCalculator::SetPassword(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::SetPassword");
  //stOutput << "Whats going on here<br>";
  this->usernamePlusPassWord=this->username;
  this->usernamePlusPassWord+=this->password;
  std::string theShaOnedString= Crypto::computeSha1outputBase64(this->usernamePlusPassWord);
  DatabaseQuery theDBQuery(theRoutines,
  "UPDATE calculatorUsers.users SET password='" + theShaOnedString +
  "' WHERE user='" + this->username + "'");
  if (!theDBQuery.flagQuerySucceeded)
    return false;
  theRoutines.comments << "<br>Query: <br>"  << theDBQuery.theQueryString << "<br> password(sha-1):<br> " << theShaOnedString;
  return true;
}

bool DatabaseRoutines::startMySQLDatabase()
{ MacroRegisterFunctionWithName("DatabaseRoutines::startMySQLDatabase");
  this->databasePassword="";
  this->databaseUser="calculator";
  this->hostname="localhost";
  this->theDatabaseName="calculatorUsers";
  this->connection=mysql_init(0); // Initialise the instance
  if(this->connection==0)
    return *this << "MySQL initialization failed.";
  //real connection to the database follows.
  this->connection=mysql_real_connect
  (this->connection, this->hostname.c_str(), this->databaseUser.c_str(), this->databasePassword.c_str(),
   this->theDatabaseName.c_str(), 0, 0, 0);
  if(this->connection==0)
    return *this << "Connection failed on: " << this->ToString();
  stOutput << "Database connection succeeded.\n";
  *this << "Database connection succeeded.\n";
  if (mysql_select_db(this->connection, this->theDatabaseName.c_str())!=0)//Note: here zero return value = success.
    return *this << "Failed to select database: " << this->theDatabaseName << ". ";
  //CANT use DatabaseQuery object as its constructor calls this method!!!!!
  if (mysql_query(this->connection, "SELECT 1 FROM users")!=0)
  { std::string createTable=
    "CREATE TABLE users(\
    user VARCHAR(50) NOT NULL PRIMARY KEY, email VARCHAR(50) NOT NULL, \
    password VARCHAR(30) NOT NULL, authenticationTokenCreationTime VARCHAR(30), \
    authenticationToken VARCHAR(30)\
    )";
    if (mysql_query(this->connection, createTable.c_str())!=0)
    { mysql_free_result( mysql_use_result(this->connection));
      return *this << "Command:<br>" << createTable << "<br>failed";
    }
    mysql_free_result( mysql_use_result(this->connection));
  }
  mysql_free_result( mysql_use_result(this->connection));
  return true;
}

bool DatabaseRoutines::innerTestLogin(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerTestLogin");
  if (!input.IsSequenceNElementS(3))
    return false;
  const Expression& userE=input[1];
  const Expression& passE=input[2];
  DatabaseRoutines theRoutines;
  UserCalculator theUser;
  if (!userE.IsOfType<std::string>(&theUser.username))
    return theCommands << "First argument of login function is not a string";
  if (!passE.IsOfType<std::string>(&theUser.password))
    return theCommands << "Second argument of login function is not a string";
  if (!theUser.TryToLogIn(theRoutines))
    return output.MakeError
    ( "Failed to login, username: " + theUser.username+ " password: " + theUser.password+
     "<hr>Comments: " + theRoutines.comments.str(), theCommands);
  return output.AssignValue(theRoutines.comments.str(), theCommands);
}

bool DatabaseRoutines::innerGetUserPassword(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerGetUserPassword");
  UserCalculator theUser;
  if (!theUser.getUser(theCommands, input))
    return false;
  std::stringstream out;
  DatabaseRoutines theRoutines;
  out << theUser.GetPassword(theRoutines);
  return output.AssignValue(out.str(), theCommands);
}

bool DatabaseRoutines::innerAddUser(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerAddUser");
  UserCalculator theUser;
  if (!theUser.getUserPassAndEmail(theCommands, input))
    return false;
  if (MathRoutines::StringBeginsWith(theUser.username, "deleted"))
    return output.MakeError("User names starting with 'deleted' are not allowed.", theCommands);
  DatabaseRoutines theRoutines;
  theUser.CreateMeIfUsernameUnique(theRoutines);
  return output.AssignValue(theRoutines.comments.str(), theCommands);
}

bool DatabaseRoutines::innerDeleteUser(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerDeleteUser");
  UserCalculator theUser, theAdmin;
  if (!theAdmin.getUserAndPass(theCommands, input))
    return false;
  theUser.username=theAdmin.username;
  theAdmin.username="admin";
  DatabaseRoutines theRoutines;
  if (!theAdmin.Authenticate(theRoutines))
    return output.MakeError("Admin authentication failed. ", theCommands);
  theUser.DeleteMe(theRoutines);
  return output.AssignValue(theRoutines.comments.str(), theCommands);
}

bool DatabaseRoutines::innerSetUserPassword(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerSetUserPassword");
  UserCalculator theUser;
  if (!theUser.getUserAndPass(theCommands, input))
    return false;
  DatabaseRoutines theRoutines;
  theUser.SetPassword(theRoutines);
  return output.AssignValue(theRoutines.comments.str(), theCommands);
}

bool UserCalculator::getUser(Calculator& theCommands, const Expression& input)
{ MacroRegisterFunctionWithName("UserCalculator::getUser");
  if (!input.IsOfType<std::string>(&this->username))
  { theCommands << "<hr>Argument " << input.ToString() << " is supposed to be a string.";
    this->username=input.ToString();
  }
  return true;
}

bool UserCalculator::getUserAndPass(Calculator& theCommands, const Expression& input)
{ MacroRegisterFunctionWithName("UserCalculator::getUserAndPass");
  if (input.children.size!=3)
    return theCommands << "UserCalculator::getUserAndPass takes as input 2 arguments (user name and password). ";
  if (!input[1].IsOfType<std::string>(&this->username))
  { theCommands << "<hr>Argument " << input[1].ToString() << " is supposed to be a string.";
    this->username=input[1].ToString();
  }
  if (!input[2].IsOfType<std::string>(&this->password))
  { theCommands << "<hr>Argument " << input[2].ToString() << " is supposed to be a string.";
    this->password=input[2].ToString();
  }
  return true;
}

bool UserCalculator::getUserPassAndSelectedColumns(Calculator& theCommands, const Expression& input)
{ MacroRegisterFunctionWithName("UserCalculator::getUserPassAndSelectedColumns");
  if (input.children.size<3)
    return theCommands << "UserCalculator::getUserPassAndSelectedColumns takes as input at least 2 arguments (user name and password). ";
  if (!input[1].IsOfType<std::string>(&this->username))
  { theCommands << "<hr>Argument " << input[1].ToString() << " is supposed to be a string.";
    this->username=input[1].ToString();
  }
  if (!input[2].IsOfType<std::string>(&this->password))
  { theCommands << "<hr>Argument " << input[2].ToString() << " is supposed to be a string.";
    this->password=input[2].ToString();
  }
  this->selectedColumns.SetSize(input.children.size-3);
  for (int i=3; i<input.children.size; i++ )
    if (!input[i].IsOfType<std::string>(& this->selectedColumns[i-3]))
    { theCommands << "<hr>Argument " << input[i].ToString() << " is supposed to be a string";
      this->selectedColumns[i-3]=input[i].ToString();
    }
  this->selectedColumnValues.initFillInObject(this->selectedColumns.size, "");
  this->selectedColumnValuesAvailable.initFillInObject(this->selectedColumns.size, "");
  return true;
}

bool UserCalculator::getUserPassAndEmail(Calculator& theCommands, const Expression& input)
{ MacroRegisterFunctionWithName("UserCalculator::getUserPassAndEmail");
  if (input.children.size!=4)
    return theCommands << "UserCalculator::getUserPassAndEmail takes as input 3 arguments (user name, email and password). ";
  if (!input[1].IsOfType<std::string>(&this->username))
  { theCommands << "<hr>Argument " << input[1].ToString() << " is supposed to be a string.";
    this->username=input[1].ToString();
  }
  if (!input[2].IsOfType<std::string>(&this->password))
  { theCommands << "<hr>Argument " << input[2].ToString() << " is supposed to be a string.";
    this->password=input[2].ToString();
  }
  if (!input[3].IsOfType<std::string>(&this->email))
  { theCommands << "<hr>Argument " << input[3].ToString() << " is supposed to be a string.";
    this->email=input[3].ToString();
  }
  return true;
}

TimeWrapper::TimeWrapper()
{
}

void TimeWrapper::ComputeTimeStringNonReadable()
{ std::stringstream out;
  out << this->theTime.tm_year << " " << this->theTime.tm_mon << " "
  << this->theTime.tm_mday
  << " " << this->theTime.tm_hour << " "
  << this->theTime.tm_min
  << " "
  << this->theTime.tm_sec;
  this->theTimeStringNonReadable=out.str();
}

void TimeWrapper::AssignLocalTime()
{ std::time_t rawtime;
  time(&rawtime);
  this->theTime=*std::gmtime(&rawtime);
  this->ComputeTimeStringNonReadable();
}

std::string TimeWrapper::ToStringHumanReadable()
{ return std::asctime(&this->theTime);
}

void TimeWrapper::operator=(const std::string& input)
{ std::stringstream inputStream;
  inputStream << input;
  inputStream.seekg(0);
  inputStream >> this->theTime.tm_year;
  inputStream >> this->theTime.tm_mon;
  inputStream >> this->theTime.tm_mday;
  inputStream >> this->theTime.tm_hour;
  inputStream >> this->theTime.tm_min;
  inputStream >> this->theTime.tm_sec;
  this->ComputeTimeStringNonReadable();
}

void UserCalculator::FetchColumns(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::FetchColumns");
  for (int i=0; i<this->selectedColumns.size; i++)
  { DatabaseQuery theQuery(theRoutines,
    "SELECT "+this->selectedColumns[i] + " FROM calculatorUsers.users WHERE user='"+
    this->username
    +"'"
    );
    if (!theQuery.flagQuerySucceeded)
      this->selectedColumnValuesAvailable[i] ="<b>Query failed - column may not exist (or some other error occurred).</b>";
    else if (!theQuery.flagQueryReturnedResult)
      this->selectedColumnValuesAvailable[i]= "<b> Query did not return a result - column may not exist.</b>";
    else
      this->selectedColumnValues[i]= theQuery.theQueryResultString;
  }
}

bool DatabaseRoutines::innerGetUserDBEntry(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerGetAuthenticationTokenCreationTime");
  UserCalculator theUser;
  if (!theUser.getUserPassAndSelectedColumns(theCommands, input))
    return false;
  DatabaseRoutines theRoutines;
  if (!theUser.Authenticate(theRoutines))
    return theCommands << theRoutines.comments.str();
  theUser.FetchColumns(theRoutines);
  return output.AssignValue(theUser.ToStringSelectedColumns(), theCommands);
}

bool DatabaseRoutines::innerGetAuthentication(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerGetAuthentication");
  UserCalculator theUser;
  if (!theUser.getUserAndPass(theCommands, input))
    return false;
  DatabaseRoutines theRoutines;
  bool result=theUser.ComputeAuthenticationToken(theRoutines);
  theCommands << theRoutines.comments.str();
  if (!result)
    return output.MakeError("Failed to authenticate. ", theCommands);
  return output.AssignValue(theUser.authenticationToken, theCommands);
}

bool DatabaseRoutines::innerTestDatabase(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerTestDatabase");
  DatabaseRoutines theRoutines;
  std::stringstream out;
  out << "Testing database ... Comments:<br>";
  theRoutines.startMySQLDatabase();
  out << theRoutines.comments.str();
  return output.AssignValue(out.str(), theCommands);
}

std::string EmailRoutines::GetCommandToSendEmailWithMailX()
{ MacroRegisterFunctionWithName("EmailRoutines::GetCommandToSendEmailWithMailX");
  std::stringstream out;
  out << "echo "
  << "\""
  << this->subject
  << "\" "
  << "| mailx -v -s "
  << "\""
  << this->emailContent
  << "\" "
  << " -c \""
  << this->ccEmail
  << "\" "
  << "-S smtp=\""
  << this->smtpWithPort
  << "\" "
  << "-S smtp-use-starttls -S smtp-auth=login -S smtp-auth-user=\""
  << this->fromEmail
  << "\" -S smtp-auth-password=\""
  << this->fromEmailAuth
  << "\" -S ssl-verify=ignore "
  << this->toEmail;
  return out.str();
}

EmailRoutines::EmailRoutines()
{ this->fromEmail="calculator.todor.milev@gmail.com";
  this->ccEmail="todor.milev@gmail.com";
  this->smtpWithPort= "smtp.gmail.com:587";
  this->fromEmailAuth=Crypto::CharsToBase64String("A good day to use a computer algebra system");
}
#endif // MACRO_use_MySQL

