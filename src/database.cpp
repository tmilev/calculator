//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "../../calculator/src/vpfHeader7DatabaseInterface_MySQL.h"
#include "../../calculator/src/vpfHeader7DatabaseShared.h"
ProjectInformationInstance ProjectInfoVpf8_1MySQLcpp(__FILE__, "MySQL interface. ");

bool DatabaseRoutinesGlobalFunctions::LoginViaDatabase
(UserCalculatorData& theUseR, std::stringstream* comments)
{ (void) comments;
  (void) theUseR;
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::LoginViaDatabase");
  DatabaseRoutines theRoutines;
  UserCalculator userWrapper;
  userWrapper.::UserCalculatorData::operator=(theUseR);
  userWrapper.currentTable="users";
  if (userWrapper.Authenticate(theRoutines, comments))
  { theUseR=userWrapper;
    return true;
  }
  if (userWrapper.enteredAuthenticationToken!="" && userWrapper.enteredActivationToken=="" && comments!=0)
  { *comments << "<b> Authentication of user: " << userWrapper.username.value
    << " with token " << userWrapper.enteredAuthenticationToken.value << " failed. </b>";
    //*comments << "<br>DEBUG: actual token: " << userWrapper.actualAuthenticationToken.value
    //<< "<br>database user: " << theRoutines.databaseUser << "<br>database name: " << theRoutines.theDatabaseName << "<br>"
    //<< "user request: " << theGlobalVariables.userCalculatorRequestType;
  }
  if (userWrapper.enteredActivationToken!="" && comments!=0)
  { //*comments << "<br>DEBUG: actual activation token: " << userWrapper.actualActivationToken.value
    //<< ". Entered activation token: " << userWrapper.enteredActivationToken.value
    //<< "<br>database user: " << theRoutines.databaseUser << "<br>database name: " << theRoutines.theDatabaseName << "<br>"
    //<< "user request: " << theGlobalVariables.userCalculatorRequestType;
  }
  std::string activationTokenUnsafe;
  if (theGlobalVariables.userCalculatorRequestType=="changePassword" ||
      theGlobalVariables.userCalculatorRequestType=="activateAccount")
    if (userWrapper.enteredActivationToken!="")
    { if (userWrapper.actualActivationToken!="activated" &&
          userWrapper.actualActivationToken!="" &&
          userWrapper.actualActivationToken!="error")
      { if (userWrapper.enteredActivationToken.value==userWrapper.actualActivationToken.value)
        { theUseR=userWrapper;
          return true;
        }
      } else if (comments!=0)
      { if (userWrapper.actualActivationToken!="error")
          *comments << "<b>Account already activated. </b>";
        else
          *comments << "<b>An error during activation ocurred.</b>";
      }
    }
  if (userWrapper.username.value=="admin" && userWrapper.enteredPassword!="")
    if (!userWrapper.Iexist(theRoutines))
    { if (comments!=0)
        *comments << "<b>First login of user admin: setting admin pass. </b>";
      userWrapper.CreateMeIfUsernameUnique(theRoutines, comments);
      userWrapper.SetColumnEntry("activationToken", "activated", theRoutines, comments);
      userWrapper.SetColumnEntry("userRole", "admin", theRoutines, comments);
      userWrapper.userRole="admin";
      theUseR=userWrapper;
      return true;
    }
  if (comments!=0)
    *comments << DatabaseRoutines::ToStringSuggestionsReasonsForFailure(userWrapper.username.value, theRoutines, userWrapper);
  return false;
#else
  return true;
#endif
}

//meld comment
#ifdef MACRO_use_MySQL

#include "../../calculator/src/vpfHeader5Crypto.h"
#include <time.h>
#include <ctime>

std::string DatabaseStrings::usersTableName="users";
std::string DatabaseStrings::userGroupLabel="userInfo";
std::string DatabaseStrings::databaseUser="ace";
std::string DatabaseStrings::theDatabaseName="aceDB";
DatabaseRoutines::DatabaseRoutines()
{ this->connection=0;
  this->MaxNumRowsToFetch=1000;
  this->flagFirstLogin=false;
  this->theDatabaseName=DatabaseStrings::theDatabaseName;
  this->databaseUser=DatabaseStrings::databaseUser;
}

bool DatabaseRoutines::RowExists
  (const MySQLdata& key, const MySQLdata& value, const MySQLdata& tableName, std::stringstream* comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::RowExists");
  if (!this->startMySQLDatabaseIfNotAlreadyStarted(comments))
    return false;
  std::stringstream theQueryStream;
  theQueryStream << "SELECT * FROM " << this->theDatabaseName << "." << tableName.GetIdentifieR()
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
  << "TABLE_SCHEMA='" << this->theDatabaseName << "' "
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
    linkStream << theGlobalVariables.DisplayNameExecutable
    << "?request=database&currentDatabaseTable="
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
  queryStream << "SELECT * FROM " << this->theDatabaseName << ".`" << tableIdentifier << "`";
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
  << "TABLE_SCHEMA='" << this->theDatabaseName << "' "
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
  << " FROM " << this->theDatabaseName << "." << tableName.GetIdentifieR() << " WHERE "
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
  "SELECT username FROM " + theRoutines.theDatabaseName + "." + theTable.GetIdentifieR() +
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
  queryStream << "INSERT INTO " << theRoutines.theDatabaseName
  << ".users(username, deadlineInfoTableName, problemInfoTableName)"
  << " VALUES("
  << this->username.GetDatA() << ", 'defaultDeadlines', 'defaultProblemInfo' "
  << ")";
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
    queryStream << "UPDATE " << this->theDatabaseName << "." << table.GetIdentifieR()
    << " SET " << columnToSet.GetIdentifieR() << "="
    << valueToSet.GetDatA() << " WHERE " << key.GetIdentifieR() << "=" << keyValue.GetDatA();
    //  stOutput << "Got to here: " << columnName << ". ";
    DatabaseQuery theDBQuery(*this, queryStream.str(), failureComments);
    //stOutput << "<hr>Fired up query:<br>" << queryStream.str();
    if (!theDBQuery.flagQuerySucceeded)
    { if (failureComments!=0)
        *failureComments << "Failed update an already existing entry in column: " << columnToSet.value << ". ";
      return false;
    }
  } else
  { std::stringstream queryStream;
    queryStream << "INSERT INTO " << this->theDatabaseName << "." << table.GetIdentifieR()
    << "(" << key.GetIdentifieR() << ", " << columnToSet.GetIdentifieR()
    << ") VALUES(" << keyValue.GetDatA() << ", " << valueToSet.GetDatA() << ")";
    DatabaseQuery theDBQuery(*this, queryStream.str(), failureComments);
    //stOutput << "<hr>Fired up query:<br>" << queryStream.str();
    if (!theDBQuery.flagQuerySucceeded)
    { if (failureComments!=0)
        *failureComments << "Failed to insert entry: " << valueToSet.GetDatA() << " in column: "
        << columnToSet.GetIdentifieR()
        << " in table: " << table.value << ". ";
      //stOutput << "Failed to insert entry in table: " << table.value << ". ";
      return false;
    }
  }
  return true;
}

bool DatabaseRoutines::startMySQLDatabase(std::stringstream* commentsOnFailure, bool* outputfirstLogin)
{ MacroRegisterFunctionWithName("DatabaseRoutines::startMySQLDatabase");
  if (theGlobalVariables.flagRunningBuiltInWebServer)
    if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    { if (commentsOnFailure!=0)
        *commentsOnFailure << "Database operations forbidden for connections not carried over ssl. ";
      return false;
    }
  this->databasePassword="";
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
    id int NOT NULL AUTO_INCREMENT PRIMARY KEY, \
    username VARCHAR(255) NOT NULL,  \
    password LONGTEXT, \
    email LONGTEXT, \
    authenticationCreationTime LONGTEXT, \
    authenticationToken LONGTEXT, \
    activationToken LONGTEXT, \
    userRole LONGTEXT, \
    userInfo LONGTEXT, \
    problemInfoTableName LONGTEXT, \
    deadlineInfoTableName LONGTEXT, \
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
  queryStream << "INSERT INTO " << this->theDatabaseName << "."
  << tableName.GetIdentifieR() << "(" << primaryKey.GetIdentifieR() << ") VALUES(" << primaryValue.GetDatA() << ")";
  DatabaseQuery theQuery(*this, queryStream.str(), &commentsOnFailure);
  return theQuery.flagQuerySucceeded;
}

std::string UserCalculator::GetActivationAddressFromActivationToken
  (const std::string& theActivationToken, const std::string& calculatorBase,
   const std::string& inputUserNameUnsafe)
{ MacroRegisterFunctionWithName("UserCalculator::GetActivationLinkFromActivationToken");
  (void) calculatorBase;
  std::stringstream out;
  out << theGlobalVariables.DisplayPathExecutable
  << "?request=activateAccount&username=" << CGI::StringToURLString(inputUserNameUnsafe)
  << "&activationToken=" << CGI::StringToURLString(theActivationToken)
  ;
  return out.str();
}

std::string DatabaseRoutines::ToStringAllUsersHTMLFormat()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringAllUsersHTMLFormat");
  std::stringstream comments;
  DatabaseQuery theQuery(*this, "SELECT * FROM " + this->theDatabaseName + ".users;", &comments);
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

bool DatabaseRoutines::PrepareClassData
(const std::string& classFileName, List<List<std::string> >& outputUserTable,
 List<std::string>& outputLabelsUserTable, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareClassData");
  DatabaseRoutines theRoutines;
  if (!theRoutines.startMySQLDatabaseIfNotAlreadyStarted(&commentsOnFailure))
  { commentsOnFailure << "<b>Failed to start database</b>";
    return false;
  }
  std::string classTableName=DatabaseRoutines::GetTableUnsafeNameUsersOfFile(classFileName);
  if (!theRoutines.TableExists(classTableName, &commentsOnFailure))
    if (!theRoutines.CreateTable
        (classTableName, "username VARCHAR(255) NOT NULL PRIMARY KEY, " +
         DatabaseStrings::userGroupLabel +" LONGTEXT ", &commentsOnFailure, 0))
      return false;
  bool tableTruncated=false;
  int numRows=-1;
  if (!DatabaseRoutinesGlobalFunctions::FetchTablE
      (outputUserTable, outputLabelsUserTable, tableTruncated, numRows, classTableName, commentsOnFailure))
  { commentsOnFailure << "<span style=\"color:red\"><b>Failed to fetch table: "
    << classTableName << "</b></span>";
    return false;
  }
  if (tableTruncated)
  { commentsOnFailure << "<span style=\"color:red\"><b>This shouldn't happen: email list truncated. "
    << "This is likely a software bug. </b></span>";
    return false;
  }
  return true;
}

#endif
