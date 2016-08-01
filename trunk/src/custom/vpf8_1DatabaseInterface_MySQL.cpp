//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "../vpfHeader7DatabaseInterface_MySQL.h"
#include "../vpfHeader7DatabaseShared.h"
ProjectInformationInstance ProjectInfoVpf8_1MySQLcpp(__FILE__, "MySQL interface. ");

bool DatabaseRoutinesGlobalFunctions::LoginViaDatabase
(const std::string& inputUsernameUnsafe, const std::string& inputPassword,
 std::string& inputOutputAuthenticationToken, std::string& outputUserRole, std::stringstream* comments)
{
 (void) inputUsernameUnsafe;
 (void) inputPassword;
 (void) inputOutputAuthenticationToken;
 (void) outputUserRole;
 (void) comments;
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::LoginViaDatabase");
  DatabaseRoutines theRoutines;
  UserCalculator theUser;
  theUser.username.value=inputUsernameUnsafe;
  theUser.currentTable="users";
  theUser.enteredPassword=inputPassword;
  theUser.enteredAuthenticationToken=inputOutputAuthenticationToken;
  if (!theRoutines.startMySQLDatabaseIfNotAlreadyStarted(comments))
  { if (comments!=0)
      *comments << "Failed to start database. ";
    return false;
  }
  if (!theRoutines.TableExists("users", comments) || theRoutines.flagFirstLogin)
  { //stOutput << "DEBUG: GOT TO HERE\n" ;
    if (theUser.enteredPassword!="" && theUser.username!="")
    { //stOutput << "DEBUG: FIRST LOGIN\n" ;
      if (comments!=0)
        *comments << "<b>First login! Setting first password as the calculator admin pass. </b>";
      theUser.CreateMeIfUsernameUnique(theRoutines, comments);
      theUser.SetColumnEntry("activationToken", "activated", theRoutines, comments);
      theUser.SetColumnEntry("userRole", "admin", theRoutines, comments);
      outputUserRole="admin";
      //stOutput << "DEBG: comments in first login: " << comments->str();
      return true;
    }
  }
  //if (comments!=0)
  //{ //*comments << "DEBUG: Attempting to login with user: " << inputUsernameUnsafe
    //<< "<br>pass: " << inputPassword
    //<< "<br>token: " << inputOutputAuthenticationToken << "<br>";
  //}
  if (theUser.Authenticate(theRoutines, comments))
  { inputOutputAuthenticationToken=theUser.actualAuthenticationToken.value;
    outputUserRole=theUser.userRole;
//    *comments << "SUCCESS <br>DEBUG The actual authenticationToken is now: " << theUser.actualAuthenticationToken.value;
    return true;
  }
  if (inputOutputAuthenticationToken!="" && comments!=0)
  { *comments << "<b> Authentication with token " << inputOutputAuthenticationToken << " failed. </b>";
//    *comments << "DEBUG the actual token is: " << theUser.actualAuthenticationToken.value;
  }
  std::string activationTokenUnsafe;
  if (theGlobalVariables.userCalculatorRequestType=="changePassword")
  { if (!theUser.FetchOneColumn("activationToken", activationTokenUnsafe, theRoutines, comments))
      *comments << "Failed to fetch activationToken column.";
    else
      if (activationTokenUnsafe!="" && activationTokenUnsafe!="activated" &&
          inputPassword==activationTokenUnsafe)
      { inputOutputAuthenticationToken="";
        outputUserRole=theUser.userRole;
        return true;
      }
  }
  inputOutputAuthenticationToken=theUser.actualAuthenticationToken.value;
  if (theUser.username=="admin" && theUser.enteredPassword!="")
    if (!theUser.Iexist(theRoutines))
    { if (comments!=0)
        *comments << "<b>First login of user admin: setting admin pass. </b>";
      theUser.CreateMeIfUsernameUnique(theRoutines, comments);
      theUser.SetColumnEntry("activationToken", "activated", theRoutines, comments);
      theUser.SetColumnEntry("userRole", "admin", theRoutines, comments);
      outputUserRole="admin";
      return true;
    }
  if (comments!=0)
    *comments << DatabaseRoutines::ToStringSuggestionsReasonsForFailure(inputUsernameUnsafe, theRoutines, theUser);
  return false;
#else
  return true;
#endif
}

//meld comment
#ifdef MACRO_use_MySQL

#include "../vpfHeader5Crypto.h"
#include <time.h>
#include <ctime>

bool DatabaseRoutines::RowExists
  (const MySQLdata& key, const MySQLdata& value, const MySQLdata& tableName, std::stringstream* comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::RowExists");
  if (!this->startMySQLDatabaseIfNotAlreadyStarted(comments))
    return false;
  std::stringstream theQueryStream;
  theQueryStream << "SELECT * FROM calculatorUsers." << tableName.GetIdentifieR()
  << " WHERE " << key.GetIdentifieR() << "=" << value.GetDatA();
  DatabaseQuery theQuery(*this, theQueryStream.str(), comments);
  return theQuery.flagQuerySucceeded && theQuery.flagQueryReturnedResult;
}

bool DatabaseRoutines::ColumnExists
(const std::string& columnNameUnsafe, const std::string& tableNameUnsafe, std::stringstream& commentsStream)
{ MacroRegisterFunctionWithName("DatabaseRoutines::ColumnExists");
  this->startMySQLDatabaseIfNotAlreadyStarted(&commentsStream);
  if (!DatabaseRoutinesGlobalFunctions::TableExists(tableNameUnsafe, commentsStream))
    return false;
  MySQLdata columnName=columnNameUnsafe;
  MySQLdata tableName=tableNameUnsafe;
  std::stringstream columnExistsStream;
  columnExistsStream << "SELECT * FROM " << "information_schema.COLUMNS WHERE "
  << "TABLE_SCHEMA='calculatorUsers' "
  << "AND TABLE_NAME='" << tableName.GetIdentifierNoQuotes() << "' "
  << "AND COLUMN_NAME='" << columnName.GetIdentifierNoQuotes() << "' ";
  DatabaseQuery theQuery(*this, columnExistsStream.str(), &commentsStream);
  return theQuery.flagQuerySucceeded && theQuery.flagQueryReturnedResult;
}

std::string DatabaseRoutines::ToStringAllTables()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringAllTables");
  std::stringstream out;
  List<std::string> tableNames;
  if (!this->FetchTableNames(tableNames, out))
  { out << " Failed to fetch table names. ";
    return out.str();
  }
  out << "<table>";
  for (int i=0; i<tableNames.size; i++)
  { std::stringstream linkStream;
    linkStream << theGlobalVariables.DisplayNameExecutableWithPath
    << "?request=browseDatabase&currentDatabaseTable="
    << CGI::StringToURLString(tableNames[i]) << "&" << theGlobalVariables.ToStringCalcArgsNoNavigation();
    out << "<tr><td><a href=\"" << linkStream.str() << "\">" << CGI::URLStringToNormal(tableNames[i])
    << "</a></td></tr>";
  }
  out << "</table>";
  return out.str();
}

bool DatabaseRoutines::FetchTableFromDatabaseIdentifier
(List<List<std::string> >& output,
 List<std::string>& outputColumnLabels,
 bool& outputWasTruncated, int& actualNumRowsIfTruncated,
 const std::string& tableIdentifier, std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::FetchTable");
  std::stringstream queryStream;
  queryStream << "SELECT * FROM calculatorUsers.`" << tableIdentifier << "`";
  DatabaseQuery theQuery(*this, queryStream.str(), &comments, this->MaxNumRowsToFetch);
  if (!theQuery.flagQuerySucceeded)
  { comments << "Query: " << queryStream.str() << " failed. ";
    return false;
  }
  output.SetSize(theQuery.allQueryResultStrings.size);
  for (int i=0; i<theQuery.allQueryResultStrings.size; i++)
  { output[i].SetSize(theQuery.allQueryResultStrings[i].size);
    for (int j=0; j<theQuery.allQueryResultStrings[i].size; j++)
      output[i][j]=CGI::URLStringToNormal(theQuery.allQueryResultStrings[i][j]);
  }
  outputWasTruncated=theQuery.flagOutputWasTruncated;
  if (outputWasTruncated)
    actualNumRowsIfTruncated=theQuery.numRowsRead;
  else
    actualNumRowsIfTruncated=output.size;
  if (!theQuery.flagQueryReturnedResult)
    comments << "The table appears to be empty: query: " << queryStream.str() << " succeeded but returned no result. ";
  theQuery.close();
  std::stringstream queryStreamFields;
  queryStreamFields
  << "SELECT `COLUMN_NAME` FROM information_schema.COLUMNS WHERE "
  << "TABLE_SCHEMA='calculatorUsers' "
  << "AND TABLE_NAME='" << tableIdentifier << "' ";
  DatabaseQuery theFieldQuery(*this, queryStreamFields.str(), &comments);
  if (!theFieldQuery.flagQuerySucceeded)
  { comments << "Query: " << queryStreamFields.str() << " failed. ";
    return false;
  }
//  stOutput << "field query: " << theFieldQuery.allQueryResultStrings.ToStringCommaDelimited();
  outputColumnLabels.SetSize(theFieldQuery.allQueryResultStrings.size);
  for (int i=0; i<theFieldQuery.allQueryResultStrings.size; i++)
    if (theFieldQuery.allQueryResultStrings[i].size>0 )
      outputColumnLabels[i]= CGI::URLStringToNormal(theFieldQuery.allQueryResultStrings[i][0]);
  return true;
}

bool DatabaseRoutines::FetchEntry
(const MySQLdata& key, const MySQLdata& valueSearchKey, const MySQLdata& tableName,
 const MySQLdata& desiredColumn, std::string& outputUnsafe, std::stringstream* failureComments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::FetchEntry");
  std::stringstream queryStream;
  queryStream << "SELECT " << desiredColumn.GetIdentifieR()
  << " FROM calculatorUsers." << tableName.GetIdentifieR() << " WHERE "
  << key.GetIdentifieR() << "="
  << valueSearchKey.GetDatA() ;
//  stOutput << " firing up query: " << queryStream.str();
  DatabaseQuery theQuery(*this, queryStream.str(), failureComments);
  outputUnsafe="";
  if (!theQuery.flagQuerySucceeded)
  { if (failureComments!=0)
      *failureComments << "<b>Query failed - column may not exist (or some other error occurred). </b>";
    return false;
  }
  if (!theQuery.flagQueryReturnedResult)
  { if (failureComments!=0)
      *failureComments << "<b>Query did not return a result - column may not exist. </b>";
    return false;
  }
  outputUnsafe= CGI::URLStringToNormal(theQuery.firstResultString);
//  stOutput << "Input entry as fetched from the system: " <<  theQuery.firstResultString
//  << "<br>When made unsafe: " << outputUnsafe << "<br>";
  return true;
}

bool UserCalculator::IamPresentInTable(DatabaseRoutines& theRoutines, const std::string& tableNameUnsafe)
{ MacroRegisterFunctionWithName("UserCalculator::Iexist");
  MySQLdata theTable=tableNameUnsafe;
  DatabaseQuery theQuery(theRoutines,
  "SELECT username FROM calculatorUsers." + theTable.GetIdentifieR() +
  " where username=" + this->username.GetDatA()
  );
  return theQuery.flagQueryReturnedResult;
}

bool UserCalculator::CreateMeIfUsernameUnique(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::CreateMeIfUsernameUnique");
  if (this->Iexist(theRoutines))
  { if (commentsOnFailure!=0)
      *commentsOnFailure << "User " << this->username.value << " already exists. ";
    return false;
  }
  if (MathRoutines::StringBeginsWith(this->username.value, "deleted"))
  { if (commentsOnFailure!=0)
      *commentsOnFailure << "You requested creation of user: " << this->username.value
      << " however user names starting with 'deleted' are not allowed. ";
    return false;
  }
  std::stringstream queryStream;
  queryStream << "INSERT INTO calculatorUsers.users(username) VALUES(" << this->username.GetDatA() << ")";
  DatabaseQuery theQuery(theRoutines, queryStream.str());
  if (this->enteredPassword=="")
    return true;
  return this->SetPassword(theRoutines, commentsOnFailure);
}

bool DatabaseRoutines::SetEntry
  (const MySQLdata& key, const MySQLdata& keyValue, const MySQLdata& table,
   const MySQLdata& columnToSet, const MySQLdata& valueToSet,
   std::stringstream* failureComments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::SetEntry");
  if (this->RowExists(key, keyValue, table, failureComments))
  { std::stringstream queryStream;
    queryStream << "UPDATE calculatorUsers." << table.GetIdentifieR()
    << " SET " << columnToSet.GetIdentifieR() << "="
    << valueToSet.GetDatA() << " WHERE " << key.GetIdentifieR() << "=" << keyValue.GetDatA();
    //  stOutput << "Got to here: " << columnName << ". ";
    DatabaseQuery theDBQuery(*this, queryStream.str(), failureComments);
//    stOutput << "<hr>Fired up query:<br>" << queryStream.str();
    if (!theDBQuery.flagQuerySucceeded)
    { if (failureComments!=0)
        *failureComments << "Failed update an already existing entry in column: " << columnToSet.value << ". ";
      return false;
    }
  } else
  { std::stringstream queryStream;
    queryStream << "INSERT INTO calculatorUsers." << table.GetIdentifieR()
    << "(" << key.GetIdentifieR() << ", " << columnToSet.GetIdentifieR()
    << ") VALUES(" << keyValue.GetDatA() << ", " << valueToSet.GetDatA() << ")";
    DatabaseQuery theDBQuery(*this, queryStream.str(), failureComments);
    //stOutput << "<hr>Fired up query:<br>" << queryStream.str();
    if (!theDBQuery.flagQuerySucceeded)
    { if (failureComments!=0)
        *failureComments << "Failed to insert entry: " << valueToSet.GetDatA() << " in column: "
        << columnToSet.GetIdentifieR()
        << " in table: " << table.value << ". ";
//      stOutput << "Failed to insert entry in table: " << this->currentTableUnsafe << ". ";
      return false;
    }
  }
  return true;
}

bool UserCalculator::FetchOneUserRow
(DatabaseRoutines& theRoutines, std::stringstream& failureStream)
{ MacroRegisterFunctionWithName("UserCalculator::FetchOneUserRow");
  if (this->currentTable=="")
    crash << "Calling UserCalculator::FetchOneUserRow with an empty table is forbidden. " << crash;
  std::stringstream queryStream;
  queryStream << "SELECT * FROM calculatorUsers." << this->currentTable.GetIdentifieR() << " WHERE "
  << "username=" << this->username.GetDatA();
//  stOutput << "quering: " << queryStream.str();
  DatabaseQuery theQuery(theRoutines, queryStream.str(), &failureStream, 5);
  if (!theQuery.flagQuerySucceeded)
  { failureStream << "Query: " << queryStream.str() << " failed. ";
    return false;
  }
  if (!theQuery.flagQueryReturnedResult)
  { failureStream << "The table appears to be empty: query: " << queryStream.str()
    << " succeeded but returned no result. ";
    return false;
  }
  if (theQuery.allQueryResultStrings.size!=1)
  { failureStream << "Could not find entries for user " << this->username.value << ".";
    return false;
  }
  this->selectedRowFieldsUnsafe.SetSize(theQuery.allQueryResultStrings[0].size);
  for (int i=0; i<this->selectedRowFieldsUnsafe.size; i++)
    this->selectedRowFieldsUnsafe[i]=CGI::URLStringToNormal(theQuery.allQueryResultStrings[0][i]);
  theQuery.close();
  std::stringstream queryStreamFields;
  queryStreamFields
  << "SELECT `COLUMN_NAME` FROM information_schema.COLUMNS WHERE "
  << "TABLE_SCHEMA='calculatorUsers' "
  << "AND TABLE_NAME='" << this->currentTable.GetIdentifierNoQuotes() << "' ";
  DatabaseQuery theFieldQuery(theRoutines, queryStreamFields.str(), &failureStream);
  if (!theFieldQuery.flagQuerySucceeded)
  { failureStream << "Query: " << queryStreamFields.str() << " failed. ";
    return false;
  }
//  stOutput << " theFieldQuery.allQueryResultStrings.size equals: " << theFieldQuery.allQueryResultStrings.size;
//  stOutput << " theQuery.allQueryResultStrings[0].size equals: " << theQuery.allQueryResultStrings[0].size;
//  stOutput << "field query: " << theFieldQuery.allQueryResultStrings.ToStringCommaDelimited();
  this->selectedRowFieldNamesUnsafe.SetSize(theFieldQuery.allQueryResultStrings.size);
  for (int i=0; i<theFieldQuery.allQueryResultStrings.size; i++)
    if (theFieldQuery.allQueryResultStrings[i].size>0 )
      this->selectedRowFieldNamesUnsafe[i]=CGI::URLStringToNormal(theFieldQuery.allQueryResultStrings[i][0]);
//  stOutput << "Got to here. this->currentTable.value is: " << this->currentTable.value
//  << ". Keys: " << this->selectedRowFieldNamesUnsafe.ToStringCommaDelimited()
//  << " vals: " << this->selectedRowFieldsUnsafe.ToStringCommaDelimited();
  if (this->currentTable=="users")
  { this->activationToken= this->GetSelectedRowEntry("activationToken");
    this->email= this->GetSelectedRowEntry("email");
    this->userRole= this->GetSelectedRowEntry("userRole");
/*    stOutput << "Got to here. Keys: " << this->selectedRowFieldNamesUnsafe.ToString()
    << " values: " << this->selectedRowFieldsUnsafe.ToString()
    << " user role: "
    << this->userRole
    << " email: " << this->email.value
    << "  act token: " << this->activationToken.value;*/
  }
  /*stOutput << "Fetched field names: " << this->selectedRowFieldNamesUnsafe.ToStringCommaDelimited()
  << " from: " << theFieldQuery.allQueryResultStrings.ToStringCommaDelimited()
  << ". All of those have values: "
  << this->selectedRowFieldsUnsafe.ToStringCommaDelimited()
  << " xtracted from " << theQuery.allQueryResultStrings.ToStringCommaDelimited();*/
  return true;
}

bool DatabaseRoutines::startMySQLDatabase(std::stringstream* commentsOnFailure, bool* outputfirstLogin)
{ MacroRegisterFunctionWithName("DatabaseRoutines::startMySQLDatabase");
  if (theGlobalVariables.flagUsingBuiltInWebServer)
    if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    { if (commentsOnFailure!=0)
        *commentsOnFailure << "Database operations forbidden for connections not carried over ssl. ";
      return false;
    }
  this->databasePassword="";
  this->databaseUser="calculator";
  this->hostname="localhost";
  this->theDatabaseName="calculatorUsers";
  this->connection=mysql_init(0); // Initialize the instance
  if(this->connection==0)
  { if (commentsOnFailure!=0)
      *commentsOnFailure << "MySQL initialization failed.";
    return false;
  }
  //real connection to the database follows.
  this->connection=mysql_real_connect
  (this->connection, this->hostname.c_str(), this->databaseUser.c_str(), this->databasePassword.c_str(),
   this->theDatabaseName.c_str(), 0, 0, 0);
  if(this->connection==0)
  { if (commentsOnFailure!=0)
      *commentsOnFailure << "Connection failed on: " << this->ToString();
    return false;
  }
  //stOutput << "Database connection succeeded.\n";
  //*this << "Database connection succeeded.\n";
  if (mysql_select_db(this->connection, this->theDatabaseName.c_str())!=0)//Note: here zero return value = success.
  { if (commentsOnFailure!=0)
      *commentsOnFailure << "Failed to select database: " << this->theDatabaseName << ". ";
    return false;
  }
  //CANT use DatabaseQuery object as its constructor calls this method!!!!!
  mysql_free_result( mysql_use_result(this->connection));
  return this->CreateTable("users", "\
    username VARCHAR(255) NOT NULL PRIMARY KEY,  \
    password LONGTEXT, \
    email LONGTEXT,\
    authenticationCreationTime LONGTEXT, \
    authenticationToken LONGTEXT , \
    activationToken LONGTEXT,\
    userRole LONGTEXT,\
    userInfo LONGTEXT, \
    problemData LONGTEXT \
    ", commentsOnFailure, outputfirstLogin);
}

bool DatabaseRoutines::InsertRow
(const std::string& primaryKeyUnsafe, const std::string& primaryValueUnsafe,
 const std::string& tableNameUnsafe, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::InsertRow");
  MySQLdata primaryKey   = primaryKeyUnsafe;
  MySQLdata primaryValue = primaryValueUnsafe;
  MySQLdata tableName    = tableNameUnsafe;
  std::stringstream queryStream;
  queryStream << "INSERT INTO calculatorUsers."
  << tableName.GetIdentifieR() << "(" << primaryKey.GetIdentifieR() << ") VALUES(" << primaryValue.GetDatA() << ")";
  DatabaseQuery theQuery(*this, queryStream.str(), &commentsOnFailure);
  return theQuery.flagQuerySucceeded;
}

std::string UserCalculator::GetActivationAddressFromActivationToken
  (const std::string& theActivationToken, const std::string& calculatorBase,
   const std::string& inputUserNameUnsafe)
{ MacroRegisterFunctionWithName("UserCalculator::GetActivationLinkFromActivationToken");
  std::stringstream out;
  out //<< "<a href=\""
  << calculatorBase
  << "?request=activateAccount&usernameHidden=" << CGI::StringToURLString(inputUserNameUnsafe)
  << "&activationToken=" << CGI::StringToURLString(theActivationToken)
  //<< "\">Activate account and set password</a>"
  ;
  return out.str();
}

std::string DatabaseRoutines::ToStringAllUsersHTMLFormat()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringAllUsersHTMLFormat");
  std::stringstream comments;
  DatabaseQuery theQuery
  (*this, "SELECT * FROM calculatorUsers.users;", &comments);
  if (!theQuery.flagQueryReturnedResult)
    return  "Query failed or returned no result. Comments: " + comments.str();
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
#endif
