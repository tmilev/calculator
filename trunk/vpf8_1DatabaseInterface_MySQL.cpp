//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifdef MACRO_use_MySQL
#include "vpfHeader7DatabaseInterface_MySQL.h"
#include "vpfHeader5Crypto.h"
ProjectInformationInstance ProjectInfoVpf8_1MySQLcpp(__FILE__, "MySQL interface. ");

std::string DatabaseRoutines::ToString()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToString");
  std::stringstream out;
  out << "Mysql, user: " << this->username << ", password: " << this->password << ", database: "
  << this->theDatabaseName;
  return out.str();
}

DatabaseRoutines::DatabaseRoutines()
{ for (unsigned i=0; i<this->password.size(); i++)
    this->password[i]=' ';
  for (unsigned i=0; i<this->usernamePlusPassWord.size(); i++)
    this->usernamePlusPassWord[i]=' ';
  this->connection=0;
}

DatabaseRoutines::~DatabaseRoutines()
{ if (this->connection!=0)
    mysql_close(this->connection);   // Close and shutdown
  this->connection=0;
}

bool DatabaseRoutines::TryToLogIn()
{ MacroRegisterFunctionWithName("DatabaseRoutines::TryToLogIn");
  if (!this->startMySQLDatabase())
    return false;
  DatabaseQuery theQuery(*this, "SELECT " +this->username + " FROM users LIMIT 1");
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
  { this->parent->comments << "mysql_store_result failed";
    return;
  }
  this->currentRow=mysql_fetch_row(this->theQueryResult);
  if (this->currentRow==0)
  { this->parent->comments << "No rows returned by the query";
    return;
  }
  this->parent->comments << "Query " << this->theQueryString << " returned rows.";
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

std::string DatabaseRoutines::GetUserPassword()
{ MacroRegisterFunctionWithName("DatabaseRoutines::GetUserPassword");
  //stOutput << "Whats going on here<br>";
  DatabaseQuery theDBQuery(*this, "SELECT password FROM calculatorUsers.users WHERE email=\"" + this->username + "\"");
  if (!theDBQuery.flagQueryReturnedResult)
    return this->comments.str();
  std::stringstream out;
  out << "password(sha-1): " << theDBQuery.theQueryResultString << "<br>comments: " << this->comments.str();
  return out.str();
}

bool DatabaseRoutines::SetUserPassword()
{ MacroRegisterFunctionWithName("DatabaseRoutines::SetUserPassword");
  //stOutput << "Whats going on here<br>";
  this->usernamePlusPassWord=this->username;
  this->usernamePlusPassWord+=this->password;
  std::string theShaOnedString= Crypto::computeSha1outputBase64(this->usernamePlusPassWord);

  DatabaseQuery theDBQuery(*this, "UPDATE calculatorUsers.users SET password='" + theShaOnedString +
                           "' WHERE email='" + this->username + "'");
  if (!theDBQuery.flagQuerySucceeded)
    return false;
  this->comments << "<br>Query: <br>"  << theDBQuery.theQueryString << "<br> password(sha-1):<br> " << theShaOnedString;
  return true;
}

bool DatabaseRoutines::startMySQLDatabase()
{ MacroRegisterFunctionWithName("DatabaseRoutines::startMySQLDatabase");
  this->password="";
  this->username="calculator";
  this->hostname="localhost";
  this->theDatabaseName="calculatorUsers";
  this->connection=mysql_init(0); // Initialise the instance
  if(this->connection==0)
    return *this << "MySQL initialization failed.";
  //real connection to the database follows.
  this->connection=mysql_real_connect
  (this->connection, this->hostname.c_str(), this->username.c_str(), this->password.c_str(),
   this->theDatabaseName.c_str(), 0, 0, 0);
  if(this->connection==0)
    return *this << "Connection failed on: " << this->ToString();
  stOutput << "Database connection succeeded.\n";
  *this << "Database connection succeeded.\n";
  if (mysql_select_db(this->connection, this->theDatabaseName.c_str())!=0)//Note: here zero return value = success.
    return *this << "Failed to select database: " << this->theDatabaseName << ". ";
  //CANT use DatabaseQuery object as its constructor calls this method!!!!!
  if (mysql_query(this->connection, "SELECT 1 FROM users")!=0)
  { if (mysql_query(this->connection, "CREATE TABLE users(email VARCHAR(50) NOT NULL PRIMARY KEY, password VARCHAR(30) NOT NULL)")!=0)
    { mysql_free_result( mysql_use_result(this->connection));
      return *this << "Command:<br>CREATE TABLE users(email VARCHAR(50) NOT NULL PRIMARY KEY, password VARCHAR(30) NOT NULL)<br>failed";
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
  if (!userE.IsOfType<std::string>(&theRoutines.username))
    return theCommands << "First argument of login function is not a string";
  if (!passE.IsOfType<std::string>(&theRoutines.password))
    return theCommands << "Second argument of login function is not a string";
  if (!theRoutines.TryToLogIn())
    return output.MakeError
    ( "Failed to login, username: " + theRoutines.username+ " password: " + theRoutines.password+
     "<hr>Comments: " + theRoutines.comments.str(), theCommands);
  return output.AssignValue(theRoutines.comments.str(), theCommands);
}

bool DatabaseRoutines::innerGetUserPassword(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerGetUserPassword");
  DatabaseRoutines theRoutines;
  if (!input.IsOfType<std::string>(&theRoutines.username))
    return theCommands << "Argument " << input.ToString() << " is supposed to be a string.";
  std::stringstream out;
  out << theRoutines.GetUserPassword();
  return output.AssignValue(out.str(), theCommands);
}

bool DatabaseRoutines::innerSetUserPassword(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerSetUserPassword");
  if (input.children.size!=3)
    return theCommands << "SetUserPassword takes as input 2 arguments (user name and password). ";
  DatabaseRoutines theRoutines;
  if (!input[1].IsOfType<std::string>(&theRoutines.username))
    return theCommands << "Argument " << input[1].ToString() << " is supposed to be a string.";
  if (!input[2].IsOfType<std::string>(&theRoutines.password))
    return theCommands << "Argument " << input[2].ToString() << " is supposed to be a string.";
  theRoutines.SetUserPassword();
  return output.AssignValue(theRoutines.comments.str(), theCommands);
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
#endif // MACRO_use_MySQL

