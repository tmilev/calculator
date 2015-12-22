//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader7DatabaseInterface_MySQL.h"
ProjectInformationInstance ProjectInfoVpf8_1MySQLcpp(__FILE__, "MySQL interface. ");

bool DatabaseRoutinesGlobalFunctions::LoginViaDatabase
(const std::string& inputUsername, const std::string& inputPassword, std::string& inputOutputAuthenticationToken)
{
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("LoginViaDatabase");
  DatabaseRoutines theRoutines;
  UserCalculator theUser;
  theUser.username=inputUsername;
  theUser.enteredPassword=inputPassword;
  theUser.enteredAuthenticationToken=inputOutputAuthenticationToken;
  //stOutput << "Attempting to login with user: " << inputUsername
  //<< " pass: " << inputPassword
  //<< " token: " << inputOutputAuthenticationToken;
  if (theUser.Authenticate(theRoutines))
  { inputOutputAuthenticationToken=theUser.actualAuthenticationToken;
    //stOutput << " SUCCESS. ";
    //stOutput << "<br>The actual authenticationToken is now: " << theUser.actualAuthenticationToken;
    return true;
  }
  inputOutputAuthenticationToken=theUser.actualAuthenticationToken;
  //stOutput << " FAIL";
  //stOutput << "<br>The actual authenticationToken is now: " << theUser.actualAuthenticationToken;
  return false;
#else
  return true;
#endif
}

#ifdef MACRO_use_MySQL
#include "vpfHeader5Crypto.h"
#include <time.h>
#include <ctime>

std::string UserCalculator::ToStringSelectedColumns()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringSelectedColumns");
  std::stringstream out;
  out << this->selectedColumns.size << " columns selected. ";
  for (int i=0; i<this->selectedColumns.size; i++)
    out << "<br>" << this->selectedColumns[i] << ": " << this->selectedColumnValues[i] << this->selectedColumnsRetrievalFailureRemarks[i];
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

UserCalculator::UserCalculator()
{ this->flagNewAuthenticationTokenComputedUserNeedsIt=false;
  this->approximateHoursSinceLastTokenWasIssued=0;
}

UserCalculator::~UserCalculator()
{ for (unsigned i=0; i<this->enteredPassword.size(); i++)
    this->enteredPassword[i]=' ';
  for (unsigned i=0; i<this->username.size(); i++)
    this->username[i]=' ';
  for (unsigned i=0; i<this->usernamePlusPassWord.size(); i++)
    this->usernamePlusPassWord[i]=' ';
  for (unsigned i=0; i<this->actualAuthenticationToken.size(); i++)
    this->actualAuthenticationToken[i]=' ';
  for (unsigned i=0; i<this->enteredAuthenticationToken.size(); i++)
    this->enteredAuthenticationToken[i]=' ';
  for (unsigned i=0; i<this->enteredShaonedSaltedPassword.size(); i++)
    this->enteredShaonedSaltedPassword[i]=' ';
  for (unsigned i=0; i<this->actualShaonedSaltedPassword.size(); i++)
    this->actualShaonedSaltedPassword[i]=' ';
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

DatabaseQuery::DatabaseQuery(DatabaseRoutines& inputParent, const std::string& inputQuery, std::stringstream* outputFailureComments)
{ MacroRegisterFunctionWithName("DatabaseQuery::DatabaseQuery");
  this->failurecomments=outputFailureComments;
  this->theQueryString=inputQuery;
  this->parent=&inputParent;
  this->flagQuerySucceeded=false;
  this->flagQueryReturnedResult=false;
  this->theQueryResult=0;
  if (this->parent->connection==0)
    if (!this->parent->startMySQLDatabase())
    { stOutput << "Failed to start database. Comments generated during the failure: " << inputParent.comments.str();
      return;
    }
  int queryError=mysql_query(this->parent->connection, this->theQueryString.c_str());
  if (queryError!=0)
  { if (this->failurecomments!=0)
    { *this->failurecomments << "Query<br>" << this->theQueryString << "<br>failed. ";
      *this->failurecomments << mysql_error(this->parent->connection);
    }
    return;
  }
  this->flagQuerySucceeded=true;
  this->theQueryResult= mysql_store_result(this->parent->connection);
  if (this->theQueryResult==0)
  { if (this->failurecomments!=0)
      *this->failurecomments << "Query succeeded, mysql_store_result returned non-zero. ";
    return;
  }
  this->allQueryResultStrings.SetSize(mysql_num_rows(this->theQueryResult));
  int numFields=mysql_num_fields(this->theQueryResult);
  if (numFields<1)
    return;
  for (int i=0; i<this->allQueryResultStrings.size; i++)
  { this->currentRow=mysql_fetch_row(this->theQueryResult);
    if (this->currentRow==0)
    { if (this->failurecomments!=0)
        *this->failurecomments << "Query result row " << i+1 << " did not return properly. " ;
      this->allQueryResultStrings[i].SetSize(0);
      continue;
    }
    this->allQueryResultStrings[i].SetSize(numFields);
    for (int fieldCounter=0; fieldCounter<numFields; fieldCounter++)
    { std::stringstream theDataCivilizer;
      theDataCivilizer << this->currentRow[fieldCounter];
      this->allQueryResultStrings[i][fieldCounter]=theDataCivilizer.str();
      if (i==0 && fieldCounter==0)
      { this->flagQueryReturnedResult=true;
        this->firstResultString=theDataCivilizer.str();
      }
    }
  }

//  stOutput << "<br>the flag: " << this->flagQueryReturnedResult;
}

DatabaseQuery::~DatabaseQuery()
{ //stOutput << "<br>DESTRUCTOR";
  if (this->theQueryResult!=0)
    mysql_free_result(this->theQueryResult);
  this->theQueryResult=0;
}

bool UserCalculator::FetchOneColumn(const std::string& columnName, std::string& output, DatabaseRoutines& theRoutines, std::string* failureComments)
{ MacroRegisterFunctionWithName("UserCalculator::FetchOneColumn");
  DatabaseQuery theQuery(theRoutines,
  "SELECT "+
  columnName
  + " FROM calculatorUsers.users WHERE user='"+
  this->username
  +"'");
  output="";
  if (!theQuery.flagQuerySucceeded)
  { if (failureComments!=0)
      *failureComments ="<b>Query failed - column may not exist (or some other error occurred).</b>";
    return false;
  }
  if (!theQuery.flagQueryReturnedResult)
  { if (failureComments!=0)
      *failureComments= "<b> Query did not return a result - column may not exist.</b>";
    return false;
  }
  output= theQuery.firstResultString;
  return true;
}

bool UserCalculator::AuthenticateWithToken(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::AuthenticateWithToken");
  if (this->enteredAuthenticationToken=="")
    return false;
  std::string authenticationTokenCreationTimeSTRING;
  std::string authenticationFailureRemarks;
  if (!this->FetchOneColumn("authenticationTokenCreationTime", authenticationTokenCreationTimeSTRING, theRoutines, &authenticationFailureRemarks))
    return theRoutines << authenticationFailureRemarks;
  this->authenticationTokenCreationTime=authenticationTokenCreationTimeSTRING;
  TimeWrapper now;
  now.AssignLocalTime();
  this->approximateHoursSinceLastTokenWasIssued=now.SubtractAnotherTimeFromMeAndGet_APPROXIMATE_ResultInHours(this->authenticationTokenCreationTime);
  if (this->approximateHoursSinceLastTokenWasIssued>3600) //3600 hours = 150 days, a bit more than the length of a semester
    return false;
  if (!this->FetchOneColumn("authenticationToken", this->actualAuthenticationToken, theRoutines, &authenticationFailureRemarks))
    return theRoutines << authenticationFailureRemarks;
  return this->enteredAuthenticationToken==this->actualAuthenticationToken;
}

bool UserCalculator::Authenticate(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  if (!this->Iexist(theRoutines))
  { if (this->username!="")
      stOutput << "user: '" << this->username << "' does not exist";
    return theRoutines << "User " << this->username << " does not exist. ";
  }
  if (this->AuthenticateWithToken(theRoutines))
    return true;
  bool result= this->AuthenticateWithUserNameAndPass(theRoutines);
  if (this->approximateHoursSinceLastTokenWasIssued>1)
    this->ResetAuthenticationToken(theRoutines);
  else if (result)
    this->FetchOneColumn("authenticationToken", this->actualAuthenticationToken, theRoutines);
  return result;
}

bool UserCalculator::AuthenticateWithUserNameAndPass(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  this->ComputeShaonedSaltedPassword();
  std::string failureRemarks;
  bool result=this->FetchOneColumn("password", this->actualShaonedSaltedPassword, theRoutines, &failureRemarks);
  if (!result)
  { //stOutput << "fail remarks: " << failureRemarks;
    //stOutput << "pass doesnt match!";
    return false;
  }
//  stOutput << "this->enteredShaonedSaltedPassword: " << this->enteredShaonedSaltedPassword
//  << ", this->actualShaonedSaltedPassword: " << this->actualShaonedSaltedPassword;
  return this->enteredShaonedSaltedPassword==this->actualShaonedSaltedPassword;
}

bool UserCalculator::ResetAuthenticationToken(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::ComputeAuthenticationToken");
  TimeWrapper now;
  now.AssignLocalTime();
  std::stringstream out;
  out << now.theTimeStringNonReadable << rand();
  this->actualAuthenticationToken=Crypto::computeSha1outputBase64(out.str());
  if (!this->SetColumnEntry("authenticationToken", this->actualAuthenticationToken, theRoutines))
    return false;
  this->flagNewAuthenticationTokenComputedUserNeedsIt=true;
  if (!this->SetColumnEntry("authenticationTokenCreationTime", now.theTimeStringNonReadable, theRoutines))
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
  out << "password(sha-1): " << theDBQuery.firstResultString << "<br>comments: " << theRoutines.comments.str();
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
  std::string userArchiveName="deleted||" + currentTime.theTimeStringNonReadable+ this->username;
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
  if (MathRoutines::StringBeginsWith(this->username, "deleted"))
    return theRoutines << "You requested creation of user: " << this->username
    << " however user names starting with 'deleted' are not allowed. ";
  DatabaseQuery theQuery(theRoutines,
  "INSERT INTO calculatorUsers.users(user, email) VALUES('" + this->username + "', '"
  + this->email + "')"
  );
  return this->SetPassword(theRoutines);
}

void UserCalculator::ComputeShaonedSaltedPassword()
{ MacroRegisterFunctionWithName("UserCalculator::ComputeShaonedSaltedPassword");
  this->usernamePlusPassWord=this->username;
  this->usernamePlusPassWord+=this->enteredPassword;
  this->enteredShaonedSaltedPassword=Crypto::computeSha1outputBase64(this->usernamePlusPassWord);
  //<-careful copying those around. We don't want to leave
  //any passwords in non-zeroed memory, even if properly freed.
  //Note the destructor of DatabaseRoutines zeroes some of the strings.
  //To do: make sure all crypto functions zero their buffers.
}

bool UserCalculator::SetColumnEntry
(const std::string& columnName, const std::string& theValue, DatabaseRoutines& theRoutines, std::stringstream* failureComments)
{ MacroRegisterFunctionWithName("UserCalculator::SetColumnEntry");
  DatabaseQuery theDBQuery(theRoutines,
  "UPDATE calculatorUsers.users SET " +
  columnName +
  "='" + theValue +
  "' WHERE user='" + this->username + "'");
  if (!theDBQuery.flagQuerySucceeded)
  { if (failureComments!=0)
      *failureComments << "Failed to set value to column: " << columnName << ". ";
    return false;
  }
  return true;
}

bool UserCalculator::SetPassword(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::SetPassword");
  //stOutput << "Whats going on here<br>";
  this->ComputeShaonedSaltedPassword();
  this->SetColumnEntry("password", this->enteredShaonedSaltedPassword, theRoutines);
  return true;
}

bool DatabaseRoutines::startMySQLDatabase()
{ MacroRegisterFunctionWithName("DatabaseRoutines::startMySQLDatabase");
  if (theGlobalVariables.flagUsingBuiltInWebServer)
    if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
      return *this << "Database operations forbidden for connections not carried over ssl. ";
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
  //stOutput << "Database connection succeeded.\n";
  *this << "Database connection succeeded.\n";
  if (mysql_select_db(this->connection, this->theDatabaseName.c_str())!=0)//Note: here zero return value = success.
    return *this << "Failed to select database: " << this->theDatabaseName << ". ";
  //CANT use DatabaseQuery object as its constructor calls this method!!!!!
  if (mysql_query(this->connection, "SELECT 1 FROM users")!=0)
  { std::string createTable=
    "CREATE TABLE users(\
    user VARCHAR(50) NOT NULL PRIMARY KEY,  \
    password VARCHAR(30) NOT NULL, \
    email VARCHAR(50) NOT NULL,\
    authenticationTokenCreationTime VARCHAR(30), \
    authenticationToken VARCHAR(30), \
    numberUnsuccessfulLoginAttempts VARCHAR(10), \
    numberSuccessfulLoginAttempts VARCHAR(10), \
    numberUnsuccessfulLoginAttemptsLast24Hours VARCHAR(10),\
    numberSuccessfulLoginAttemptsLast24Hours VARCHAR(10)\
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
  if (!passE.IsOfType<std::string>(&theUser.enteredPassword))
    return theCommands << "Second argument of login function is not a string";
  if (!theUser.TryToLogIn(theRoutines))
    return output.MakeError
    ( "Failed to login, username: " + theUser.username+ " password: " + theUser.enteredPassword +
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
  theAdmin.username=theGlobalVariables.userCalculatorAdmin;
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
  { theCommands << "<hr>Argument " << input[1].ToString() << " is supposed to be a string. ";
    this->username=input[1].ToString();
  }
  if (!input[2].IsOfType<std::string>(&this->enteredPassword))
  { theCommands << "<hr>Argument " << input[2].ToString() << " is supposed to be a string. ";
    this->enteredPassword=input[2].ToString();
    this->enteredAuthenticationToken=this->enteredPassword;
  }
  return true;
}

bool UserCalculator::getUserPassAndSelectedColumns(Calculator& theCommands, const Expression& input)
{ MacroRegisterFunctionWithName("UserCalculator::getUserPassAndSelectedColumns");
  if (input.children.size<3)
    return theCommands << "UserCalculator::getUserPassAndSelectedColumns takes as input at least 2 arguments (user name and password). ";
  bool result = this->getUserPassAndExtraData(theCommands, input, this->selectedColumns);
  this->selectedColumnValues.initFillInObject(this->selectedColumns.size, "");
  this->selectedColumnsRetrievalFailureRemarks.initFillInObject(this->selectedColumns.size, "");
  return result;
}

bool UserCalculator::getUserPassAndExtraData(Calculator& theCommands, const Expression& input, List<std::string>& outputData)
{ MacroRegisterFunctionWithName("UserCalculator::getUserPassAndExtraData");
  if (input.children.size<4)
    return theCommands << "UserCalculator::getUserPassAndExtraData takes as input at least 2 arguments (user name and password). ";
  if (!input[1].IsOfType<std::string>(&this->username))
  { theCommands << "<hr>Argument " << input[1].ToString() << " is supposed to be a string.";
    this->username=input[1].ToString();
  }
  if (!input[2].IsOfType<std::string>(&this->enteredPassword))
  { theCommands << "<hr>Argument " << input[2].ToString() << " is supposed to be a string.";
    this->enteredPassword=input[2].ToString();
  }
  outputData.SetSize(input.children.size-3);
  for (int i=3; i<input.children.size; i++ )
    if (!input[i].IsOfType<std::string>(& this->selectedColumns[i-3]))
    { theCommands << "<hr>Argument " << input[i].ToString() << " is supposed to be a string";
      outputData[i-3]=input[i].ToString();
    }
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
  if (!input[2].IsOfType<std::string>(&this->enteredPassword))
  { theCommands << "<hr>Argument " << input[2].ToString() << " is supposed to be a string.";
    this->enteredPassword=input[2].ToString();
    this->enteredAuthenticationToken=this->enteredPassword;
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

double TimeWrapper::SubtractAnotherTimeFromMeAndGet_APPROXIMATE_ResultInHours(const TimeWrapper& other)
{ //This is a simple function aimed at computing expiration of authentication tokens and
  //does not need to be precise.
  //To make matters simple, the function approximates:
  //every year by 365 days
  //every month by 30.4 days
  double result=0;
  result+=(this->theTime.tm_year - other.theTime.tm_year)*365*24;
  result+=(this->theTime.tm_mon  - other.theTime.tm_mon )*30.4*24;
  result+=(this->theTime.tm_mday - other.theTime.tm_mday)*30.4*24;
  result+=(this->theTime.tm_hour - other.theTime.tm_hour);
  result+=(this->theTime.tm_min  - other.theTime.tm_min);
  //seconds are ignored
  return result;
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
    this->FetchOneColumn(this->selectedColumns[i], this->selectedColumnValues[i], theRoutines, &this->selectedColumnsRetrievalFailureRemarks[i]);
}

std::string DatabaseRoutines::ToStringAllUsersHTMLFormat()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringAllUsersHTMLFormat");
  DatabaseQuery theQuery
  (*this, "SELECT * FROM calculatorUsers.users;", &this->comments);
  if (!theQuery.flagQueryReturnedResult)
    return  "Query failed or returned no result. Comments: " + this->comments.str();
  std::stringstream out;
  out << "Table users has " << theQuery.allQueryResultStrings.size << " entries. ";
  out << "<table border=\"1\"><tr>";
  out
  << " <td>user</td>"
  << "<td>salted shaoned password</td>"
  << "<td>email</td>"
  << "<td>Authentication token time stamp</td>"
  << "<td>Authentication token</td>"
  << "<td>Number unsuccessful logins</td>"
  << "<td>Number successful logins</td>"
  << "<td>Number unsuccessful logins, last 24 hours</td>"
  << "<td>Number successful logins, last 24 hours </td>"
  ;
  out << "</tr>";
  for (int i=0; i<theQuery.allQueryResultStrings.size; i++)
  { out << "<tr>";
    for (int j=0; j<theQuery.allQueryResultStrings[i].size; j++)
      out << "<td>" << theQuery.allQueryResultStrings[i][j] << "</td>";
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

bool DatabaseRoutines::innerGetUserDetails(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerGetUserDetails");
  UserCalculator theUser;
  if (!theUser.getUserAndPass(theCommands, input))
    return false;
  DatabaseRoutines theRoutines;
  if (theUser.username!=theGlobalVariables.userCalculatorAdmin)
    return output.AssignValue
    ((std::string)"At the moment, the GetUserDetails function is implemented only for the admin user. ", theCommands);
  if (!theUser.Authenticate(theRoutines))
    return theCommands << "Authentication failed. " << theRoutines.comments.str();
  return output.AssignValue(theRoutines.ToStringAllUsersHTMLFormat(), theCommands);
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
  if (!theUser.Authenticate(theRoutines))
    return output.MakeError("Failed to authenticate. ", theCommands);
  return output.AssignValue(theUser.actualAuthenticationToken, theCommands);
}

bool DatabaseRoutines::innerAddTeachingClass(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerAddTeachingClass");
  UserCalculator theUser;
  List<std::string> className;
  if (!theUser.getUserPassAndExtraData(theCommands, input, className))
    return false;
  DatabaseRoutines theRoutines;
  if (!theUser.Authenticate(theRoutines))
    return output.MakeError("Failed to authenticate. ", theCommands);
  DatabaseQuery theQuery
  (theRoutines, "");
  return output.AssignValue(theUser.actualAuthenticationToken, theCommands);
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

