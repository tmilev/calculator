//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader7DatabaseInterface_MySQL.h"
ProjectInformationInstance ProjectInfoVpf8_1MySQLcpp(__FILE__, "MySQL interface. ");

bool DatabaseRoutinesGlobalFunctions::LoginViaDatabase
(const std::string& inputUsername, const std::string& inputPassword, std::string& inputOutputAuthenticationToken, std::stringstream* comments)
{
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::LoginViaDatabase");
  DatabaseRoutines theRoutines;
  UserCalculator theUser;
  theUser.usernameUnsafe=inputUsername;
  theUser.enteredPassword=inputPassword;
  theUser.enteredAuthenticationTokenUnsafe=inputOutputAuthenticationToken;
//  stOutput << "Attempting to login with user: " << inputUsername
//  << " pass: " << inputPassword
//  << " token: " << inputOutputAuthenticationToken;

  if (theUser.Authenticate(theRoutines, true))
  { inputOutputAuthenticationToken=theUser.actualAuthenticationTokeNUnsafe;
//    stOutput << " SUCCESS. ";
//    stOutput << "<br>The actual authenticationToken is now: " << theUser.actualAuthenticationToken;
    return true;
  }
  if (inputOutputAuthenticationToken!="" && comments!=0)
  { *comments << "<b> Authentication with token " << inputOutputAuthenticationToken << " failed. </b>";
    //*comments << "the actual token is: " << theUser.actualAuthenticationTokeNUnsafe;
  }

  inputOutputAuthenticationToken=theUser.actualAuthenticationTokeNUnsafe;
  theUser.usernameUnsafe=theGlobalVariables.userCalculatorAdmin;
  if (!theUser.Iexist(theRoutines, true))
  { if (theUser.enteredPassword!="")
    { stOutput << "<b>First login! Setting first password as the calculator admin pass.</b>";
      theUser.CreateMeIfUsernameUnique(theRoutines, false);
    }
    return true;
  }
//  stOutput << " <b>login failed, desired user:" + theUser.username +"</b>";
//  stOutput << "<br>The actual authenticationToken is now: " << theUser.actualAuthenticationToken;
  return false;
#else
  return true;
#endif
}

bool DatabaseRoutinesGlobalFunctions::LogoutViaDatabase()
{
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::LogoutViaDatabase");
  if (!theGlobalVariables.flagLoggedIn)
    return true;
  DatabaseRoutines theRoutines;
  UserCalculator theUser;
  theUser.usernameUnsafe=theGlobalVariables.userDefault;
  theUser.SetCurrentTable("users");
  theUser.ResetAuthenticationToken(theRoutines, true);
  theGlobalVariables.SetWebInput("authenticationToken", "");
  return true;
#else
  return true;
#endif
}

bool DatabaseRoutinesGlobalFunctions::SetPassword
(const std::string& inputUsername, const std::string& inputNewPassword,
 std::stringstream& comments)
{
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::SetPassword");
  if(!theGlobalVariables.flagLoggedIn)
  { comments << "Changing passwords allowed for logged-in users only. ";
    return false;
  }
  DatabaseRoutines theRoutines;
  UserCalculator theUser;
  theUser.usernameUnsafe=inputUsername;
  theUser.enteredPassword=inputNewPassword;
  return theUser.SetPassword(theRoutines, true, comments);
#else
  return false;
#endif // MACRO_use_MySQL
}

bool DatabaseRoutinesGlobalFunctions::SetEntry
(const std::string& inputUsername, const std::string& tableName, const std::string& keyName,
 const std::string& value, std::stringstream& comments)
{
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::SetEntry");
  if (!theGlobalVariables.flagLoggedIn)
    return false;
  DatabaseRoutines theRoutines;
  UserCalculator theUser;
  theUser.usernameUnsafe=inputUsername;
//  stOutput << "setting table name to: " << tableName << "<br>";
  theUser.SetCurrentTable(tableName);

  return theUser.SetColumnEntry(keyName, value, theRoutines, true, &comments);
#else
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctions::UserDefaultHasInstructorRights()
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::UserDefaultHasInstructorRights");
  if (theGlobalVariables.UserDefaultHasAdminRights())
    return true;
#ifdef MACRO_use_MySQL
  if (!theGlobalVariables.flagLoggedIn)
    return false;
  std::stringstream comments;
  if (!DatabaseRoutinesGlobalFunctions::TableExists("instructors", comments))
    DatabaseRoutinesGlobalFunctions::CreateTable("instructors", comments);
  std::string notUsed;
  return DatabaseRoutinesGlobalFunctions::FetchEntry(theGlobalVariables.userDefault, "instructors", "users", notUsed, comments);
#else
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctions::CreateTable
(const std::string& tableName, std::stringstream& comments)
{
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::CreateTable");
  if (!theGlobalVariables.flagLoggedIn)
    return false;
  DatabaseRoutines theRoutines;
  theRoutines.startMySQLDatabaseIfNotAlreadyStarted();
  return theRoutines.CreateTable(tableName,  "user VARCHAR(50) NOT NULL PRIMARY KEY  ", &comments);
#else
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctions::ColumnExists
(const std::string& columnNameUnsafe, const std::string& tableNameUnsafe, std::stringstream& commentsStream)
{
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::ColumnExists");
  DatabaseRoutines theRoutines;
  return theRoutines.ColumnExists(columnNameUnsafe, tableNameUnsafe, commentsStream);

#else
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctions::CreateColumn
(const std::string& columnNameUnsafe, const std::string& tableNameUnsafe,
 std::stringstream& commentsOnCreation)
{
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::CreateColumn");
  DatabaseRoutines theRoutines;
  return theRoutines.CreateColumn(columnNameUnsafe, tableNameUnsafe, commentsOnCreation);
#else
  return false;
#endif

}

bool DatabaseRoutinesGlobalFunctions::TableExists
(const std::string& tableName, std::stringstream& comments)
{
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::TableExists");
  if (!theGlobalVariables.flagLoggedIn)
    return false;
  DatabaseRoutines theRoutines;
  if (!theRoutines.startMySQLDatabaseIfNotAlreadyStarted())
    crash << "Failed to start database. " << crash;
  return theRoutines.TableExists(tableName);
#else
  return true;
#endif
}

bool DatabaseRoutinesGlobalFunctions::FetchEntry
(const std::string& inputUsername, const std::string& tableName, const std::string& keyName,
 std::string& output, std::stringstream& comments)
{
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::FetchEntry");
  if (!theGlobalVariables.flagLoggedIn)
    return false;
  DatabaseRoutines theRoutines;
  UserCalculator theUser;
  theUser.usernameUnsafe=inputUsername;
  theUser.SetCurrentTable(tableName);
  return theUser.FetchOneColumn(keyName, output, theRoutines, true, &comments);
#else
  return true;
#endif
}

#ifdef MACRO_use_MySQL
#include "vpfHeader5Crypto.h"
#include <time.h>
#include <ctime>

bool DatabaseRoutines::ColumnExists
(const std::string& columnNameUnsafe, const std::string& tableNameUnsafe, std::stringstream& commentsStream)
{ MacroRegisterFunctionWithName("DatabaseRoutines::ColumnExists");
  this->startMySQLDatabaseIfNotAlreadyStarted();
  if (!DatabaseRoutinesGlobalFunctions::TableExists(tableNameUnsafe, commentsStream))
    return false;
  std::string columnNameSafe=CGI::StringToURLString(columnNameUnsafe);
  std::string tableNameSafe=CGI::StringToURLString(tableNameUnsafe);
  std::stringstream columnExistsStream;
  columnExistsStream << "SELECT * FROM " << "information_schema.COLUMNS WHERE "
  << "TABLE_SCHEMA='calculatorUsers' "
  << "AND TABLE_NAME='" << tableNameSafe << "' "
  << "AND COLUMN_NAME='" << columnNameSafe << "' ";
  stOutput << "firing query: " << columnExistsStream.str();
  DatabaseQuery theQuery(*this, columnExistsStream.str());
  return theQuery.flagQuerySucceeded && theQuery.flagQueryReturnedResult;
}

bool DatabaseRoutines::CreateColumn
(const std::string& columnNameUnsafe, const std::string& tableNameUnsafe,
 std::stringstream& commentsOnCreation)
{ MacroRegisterFunctionWithName("DatabaseRoutines::CreateColumn");
//  stOutput << "Creating column pt 1.";
  this->startMySQLDatabaseIfNotAlreadyStarted();
  if (!this->TableExists(tableNameUnsafe))
    return false;
//  stOutput << "Creating column pt 2.";
  std::string columnNameSafe=CGI::StringToURLString(columnNameUnsafe);
  std::string tableNameSafe=CGI::StringToURLString(tableNameUnsafe);
//  stOutput << "<hr>BEFORE alter command";
  std::stringstream commandStream;
  commandStream << "ALTER TABLE \"" << tableNameSafe << "\""
  << " ADD \"" << columnNameSafe << "\" "
  << "LONGTEXT";
//  stOutput << "<br>got to here<br>hrbr<br>";
  DatabaseQuery theQuery(*this, commandStream.str());
//  stOutput << "<br>AFTER alter command: " << theRoutines.comments.str() << "<hr>";
  return theQuery.flagQuerySucceeded;

}

std::string UserCalculator::ToStringSelectedColumns()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringSelectedColumns");
  std::stringstream out;
  out << this->selectedColumnsUnsafe.size << " columns selected. ";
  for (int i=0; i<this->selectedColumnsUnsafe.size; i++)
    out << "<br>" << this->selectedColumnsUnsafe[i] << ": " << this->selectedColumnsUnsafe[i] << this->selectedColumnsRetrievalFailureRemarks[i];
  return out.str();
}

std::string DatabaseRoutines::ToString()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToString");
  std::stringstream out;
  out << "Database name: " << this->theDatabaseName;
  return out.str();
}

bool UserCalculator::SetCurrentTable(const std::string& inputTableNameUnsafe)
{ this->currentTableUnsafe=inputTableNameUnsafe;
  this->currentTableSafe=CGI::StringToURLString(this->currentTableUnsafe);
  return true;
}

std::string UserCalculator::ToString()
{ MacroRegisterFunctionWithName("UserCalculator::ToString");
  std::stringstream out;
  out << "Calculator user: " << this->usernameUnsafe;
  return out.str();
}

UserCalculator::UserCalculator()
{ this->flagNewAuthenticationTokenComputedUserNeedsIt=false;
  this->approximateHoursSinceLastTokenWasIssued=2;
}

UserCalculator::~UserCalculator()
{ for (unsigned i=0; i<this->enteredPassword.size(); i++)
    this->enteredPassword[i]=' ';
  for (unsigned i=0; i<this->usernamePlusPassWord.size(); i++)
    this->usernamePlusPassWord[i]=' ';
  for (unsigned i=0; i<this->enteredShaonedSaltedPassword.size(); i++)
    this->enteredShaonedSaltedPassword[i]=' ';
  for (unsigned i=0; i<this->actualShaonedSaltedPassword.size(); i++)
    this->actualShaonedSaltedPassword[i]=' ';
}

std::string DatabaseRoutines::ToStringTable(const std::string& inputTableName)
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringTable");
  std::stringstream out, comments;
  List<List<std::string> > theTable;
  List<std::string> columnLabels;
  bool wasTruncated=false;
  int actualNumEntries=-1;
  if (!this->FetchTable(theTable, columnLabels, wasTruncated, actualNumEntries, inputTableName, comments))
  { out << "<b>Failed to fetch table: " << inputTableName << ".</b> Comments: " << comments.str();
    return out.str();
  }
  out << "Table has " << actualNumEntries << " entries. ";
  if (wasTruncated)
    out << "<b>The number of entries was truncated to " << theTable.size << ". <b>";
  out << "<table><tr>";
  for (int i=0; i<columnLabels.size; i++)
    out << "<td>" << CGI::URLStringToNormal(columnLabels[i]) << "</td>";
  out << "</tr>";
  for (int i=0; i<theTable.size; i++)
  { out << "<tr>";
    for (int j=0; j<theTable[i].size; j++)
      out << "<td>" << CGI::URLStringToNormal( theTable[i][j]) << "</td>";
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
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
    linkStream << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=browseDatabase&currentDatabaseTable="
    << tableNames[i] << "&" << theGlobalVariables.ToStringCalcArgsNoNavigation();
    out << "<tr><td><a href=\"" << linkStream.str() << "\">" << CGI::URLStringToNormal(tableNames[i]) << "</a></td></tr>";
  }
  out << "</table>";
  return out.str();
}

bool DatabaseRoutines::FetchTableNames(List<std::string>& output, std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::FetchTableNames");
  std::stringstream queryStream;
  queryStream << "SHOW TABLES";
  DatabaseQuery theQuery(*this, queryStream.str(), &comments);
  if (!theQuery.flagQuerySucceeded)
    return false;
  output.SetSize(theQuery.allQueryResultStrings.size);
//  stOutput << "show tables: " << theQuery.allQueryResultStrings.ToStringCommaDelimited();
  for (int i=0; i<theQuery.allQueryResultStrings.size; i++)
    if (theQuery.allQueryResultStrings[i].size>0)
      output[i]=theQuery.allQueryResultStrings[i][0];
  return true;
}

std::string DatabaseRoutines::ToStringCurrentTableHTML()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringCurrentTableHTML");
  std::string currentTable=CGI::URLStringToNormal( theGlobalVariables.GetWebInput("currentDatabaseTable"));
  if (currentTable=="")
    return this->ToStringAllTables();
  return this->ToStringTable(currentTable);
}

bool DatabaseRoutines::FetchTable
(List<List<std::string> >& output,
 List<std::string>& outputColumnLabels,
 bool& outputWasTruncated, int& actualNumRowsIfTruncated,
 const std::string& tableNameUnsafe, std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::FetchTable");
  std::string tableNameSafe=CGI::StringToURLString(tableNameUnsafe);
  std::stringstream queryStream;
  queryStream << "SELECT * FROM calculatorUsers.\"" << tableNameSafe << "\"";
  DatabaseQuery theQuery(*this, queryStream.str(), &comments, this->MaxNumRowsToFetch);
  if (!theQuery.flagQuerySucceeded)
  { comments << "Query: " << queryStream.str() << " failed. ";
    return false;
  }
  output=theQuery.allQueryResultStrings;
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
  << "SELECT \"COLUMN_NAME\" FROM information_schema.COLUMNS WHERE "
  << "TABLE_SCHEMA='calculatorUsers' "
  << "AND TABLE_NAME='" << tableNameSafe << "' ";
  DatabaseQuery theFieldQuery(*this, queryStreamFields.str(), &comments);
  if (!theFieldQuery.flagQuerySucceeded)
  { comments << "Query: " << queryStreamFields.str() << " failed. ";
    return false;
  }
//  stOutput << "field query: " << theFieldQuery.allQueryResultStrings.ToStringCommaDelimited();
  outputColumnLabels.SetSize(theFieldQuery.allQueryResultStrings.size);
  for (int i=0; i<theFieldQuery.allQueryResultStrings.size; i++)
    if (theFieldQuery.allQueryResultStrings[i].size>0 )
      outputColumnLabels[i]=theFieldQuery.allQueryResultStrings[i][0];
  return true;
}

DatabaseRoutines::DatabaseRoutines()
{ this->connection=0;
  this->MaxNumRowsToFetch=1000;
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
  this->ComputeSafeObjectNames();
  std::stringstream queryStream;
  queryStream << "SELECT " << this->usernameSafe << " FROM users LIMIT 1";
  DatabaseQuery theQuery(theRoutines, queryStream.str());
  if (!theQuery.flagQueryReturnedResult)
    return false;
  return false;
}

DatabaseQuery::DatabaseQuery(DatabaseRoutines& inputParent, const std::string& inputQuery, std::stringstream* outputFailureComments, int inputMaxNumRowsToRead)
{ MacroRegisterFunctionWithName("DatabaseQuery::DatabaseQuery");
  this->failurecomments=outputFailureComments;
  this->theQueryString=inputQuery;
  this->parent=&inputParent;
  this->flagQuerySucceeded=false;
  this->flagQueryReturnedResult=false;
  this->theQueryResult=0;
  this->flagOutputWasTruncated=false;
  this->MaxNumRowsToRead=inputMaxNumRowsToRead;
  this->numRowsRead=0;
//  stOutput << "<hr> querying: " << inputQuery;
  if (this->parent->connection==0)
    if (!this->parent->startMySQLDatabase())
    { stOutput << "Failed to start database. Comments generated during the failure: " << inputParent.comments.str();
      return;
    }
//  stOutput << "got to here in the queery query part -1";
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
  //stOutput << "and even to here";
  if (this->theQueryResult==0)
  { if (this->failurecomments!=0)
      *this->failurecomments << "Query succeeded, mysql_store_result returned non-zero. ";
    return;
  }
  this->numRowsRead=mysql_num_rows(this->theQueryResult);
  int numRowsToFetch=this->numRowsRead;
  if (numRowsToFetch>this->MaxNumRowsToRead)
  { this->flagOutputWasTruncated=true;
    numRowsToFetch=this->MaxNumRowsToRead;
  }
  this->allQueryResultStrings.SetSize(numRowsToFetch);
  int numFields=mysql_num_fields(this->theQueryResult);
  if (numFields<1)
    return;
//  stOutput << "here i'm now...";
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

void DatabaseQuery::close()
{ if (this->theQueryResult!=0)
    mysql_free_result(this->theQueryResult);
  this->theQueryResult=0;
}

DatabaseQuery::~DatabaseQuery()
{ //stOutput << "<br>DESTRUCTOR";
  this->close();
}

bool UserCalculator::FetchOneColumn
(const std::string& columnNameUnsafe, std::string& outputUnsafe,
 DatabaseRoutines& theRoutines, bool recomputeSafeEntries, std::stringstream* failureComments)
{ MacroRegisterFunctionWithName("UserCalculator::FetchOneColumn");
  if (recomputeSafeEntries)
    this->ComputeSafeObjectNames();
  std::stringstream queryStream;
  queryStream << "SELECT \"" << CGI::StringToURLString(columnNameUnsafe)
  << "\" FROM calculatorUsers.\"" << this->currentTableSafe << "\" WHERE user='"
  << this->usernameSafe << "'";
  DatabaseQuery theQuery(theRoutines, queryStream.str());
  outputUnsafe="";
  if (!theQuery.flagQuerySucceeded)
  { if (failureComments!=0)
      *failureComments << "<b>Query failed - column may not exist (or some other error occurred).</b>";
    return false;
  }
  if (!theQuery.flagQueryReturnedResult)
  { if (failureComments!=0)
      *failureComments << "<b> Query did not return a result - column may not exist.</b>";
    return false;
  }
  outputUnsafe= CGI::URLStringToNormal(theQuery.firstResultString);
//  stOutput << "Input entry as fetched from the system: " <<  theQuery.firstResultString
//  << "<br>When made unsafe: " << outputUnsafe << "<br>";
  return true;
}

bool UserCalculator::AuthenticateWithToken(DatabaseRoutines& theRoutines, bool recomputeSafeEntries)
{ MacroRegisterFunctionWithName("UserCalculator::AuthenticateWithToken");
  this->SetCurrentTable("users");
  if (recomputeSafeEntries)
    this->ComputeSafeObjectNames();
  if (this->enteredAuthenticationTokenUnsafe=="")
    return false;
  std::string authenticationTokenCreationTimeSTRING;
  std::stringstream authenticationFailureRemarks;
  if (!this->FetchOneColumn("authenticationTokenCreationTime", authenticationTokenCreationTimeSTRING,
                            theRoutines, false, &authenticationFailureRemarks))
  { stOutput << "Failed to fetch authentication token creation time: " << authenticationFailureRemarks.str();
    return theRoutines << authenticationFailureRemarks;
  }
  this->authenticationTokenCreationTime=authenticationTokenCreationTimeSTRING;
  TimeWrapper now;
  now.AssignLocalTime();
  this->approximateHoursSinceLastTokenWasIssued=now.SubtractAnotherTimeFromMeAndGet_APPROXIMATE_ResultInHours
  (this->authenticationTokenCreationTime);
//  stOutput << "approx hours since token issued: " << this->approximateHoursSinceLastTokenWasIssued;
//  if (this->approximateHoursSinceLastTokenWasIssued>3600 || this->approximateHoursSinceLastTokenWasIssued<=0) //3600 hours = 150 days, a bit more than the length of a semester
//    return false;
  if (!this->FetchOneColumn("authenticationToken", this->actualAuthenticationTokeNUnsafe,
                            theRoutines, false, &authenticationFailureRemarks))
    return theRoutines << authenticationFailureRemarks;
  return this->enteredAuthenticationTokenUnsafe==this->actualAuthenticationTokeNUnsafe;
}

bool UserCalculator::Authenticate(DatabaseRoutines& theRoutines, bool recomputeSafeEntries)
{ MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  if (recomputeSafeEntries)
    this->ComputeSafeObjectNames();
  if (!this->Iexist(theRoutines, false))
  { //if (this->username!="")
    //  stOutput << "user: '" << this->username << "' does not exist";
    return theRoutines << "User " << this->usernameUnsafe << " does not exist. ";
  }
  if (this->AuthenticateWithToken(theRoutines, false))
    return true;
  bool result= this->AuthenticateWithUserNameAndPass(theRoutines, false);
//  stOutput << "Approximate hours since last token issued: " << this->approximateHoursSinceLastTokenWasIssued << ". ";
  this->SetCurrentTable("users");
//  if (this->approximateHoursSinceLastTokenWasIssued>1 || this->approximateHoursSinceLastTokenWasIssued<=0)
  this->ResetAuthenticationToken(theRoutines, false);
//  else if (result)
//    this->FetchOneColumn("authenticationToken", this->actualAuthenticationTokeNUnsafe, theRoutines, false, 0);
  return result;
}

bool UserCalculator::AuthenticateWithUserNameAndPass(DatabaseRoutines& theRoutines, bool recomputeSafeEntries)
{ MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  if (recomputeSafeEntries)
    this->ComputeSafeObjectNames();
  this->ComputeShaonedSaltedPassword(false);
  std::stringstream failureRemarks;
  bool result=this->FetchOneColumn("password", this->actualShaonedSaltedPassword, theRoutines, false, &failureRemarks);
  if (!result)
  { //stOutput << "fail remarks: " << failureRemarks;
    //stOutput << "pass doesnt match!";
    return false;
  }
//  stOutput << "this->enteredShaonedSaltedPassword: " << this->enteredShaonedSaltedPassword
//  << ", this->actualShaonedSaltedPassword: " << this->actualShaonedSaltedPassword;
  return this->enteredShaonedSaltedPassword==this->actualShaonedSaltedPassword;
}

bool UserCalculator::ResetAuthenticationToken(DatabaseRoutines& theRoutines, bool recomputeSafeEntries)
{ MacroRegisterFunctionWithName("UserCalculator::ComputeAuthenticationToken");
  if (recomputeSafeEntries)
    this->ComputeSafeObjectNames();
  TimeWrapper now;
//  stOutput << "Resetting authentication token. ";
  now.AssignLocalTime();
  std::stringstream out;
  out << now.theTimeStringNonReadable << rand();
  this->actualAuthenticationTokeNUnsafe=Crypto::computeSha1outputBase64(out.str());
  std::stringstream failure;
  if (!this->SetColumnEntry("authenticationToken", this->actualAuthenticationTokeNUnsafe, theRoutines, false, &failure))
  { stOutput << "Couldn't set column entry for token." << failure.str();
    return false;
  }
  this->flagNewAuthenticationTokenComputedUserNeedsIt=true;
  if (!this->SetColumnEntry("authenticationTokenCreationTime", now.theTimeStringNonReadable, theRoutines, false, &failure))
  { stOutput << "Couldn't set column entry for creation time." << failure.str();
    return false;
  }
  //DatabaseQuery theQuery(*this, "SELECT authenticationTokenCreationTime FROM calculatorUsers.users WHERE user=\""+this->username + "\"");
  //if (theQuery.flagQueryReturnedResult)
  //  this->authentication="authenticationTokenCreationTime: "
  return true;
}

std::string UserCalculator::GetPassword(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::GetUserPassword");
  std::string storedShaonedSaltedPass;
  std::stringstream out;
  if (!this->FetchOneColumn("password", storedShaonedSaltedPass, theRoutines, true, &theRoutines.comments))
    return theRoutines.comments.str();
  out << "password(sha-1): " << storedShaonedSaltedPass << "<br>comments: " << theRoutines.comments.str();
  return out.str();
}

bool UserCalculator::Iexist(DatabaseRoutines& theRoutines, bool recomputeSafeEntries)
{ MacroRegisterFunctionWithName("UserCalculator::Iexist");
  if (recomputeSafeEntries)
    this->ComputeSafeObjectNames();
  DatabaseQuery theQuery(theRoutines,
  "SELECT user FROM calculatorUsers.users where user='" + this->usernameSafe + "' "
  );
  return theQuery.flagQueryReturnedResult;
}

bool UserCalculator::DeleteMe(DatabaseRoutines& theRoutines, bool recomputeSafeEntries)
{ MacroRegisterFunctionWithName("UserCalculator::DeleteMe");
  if (recomputeSafeEntries)
    this->ComputeSafeObjectNames();
  if (MathRoutines::StringBeginsWith(this->usernameSafe, "deleted"))
    return theRoutines << "Usernames starting with 'deleted' cannot be deleted from the calculator interface. ";
  if (!this->Iexist(theRoutines, false))
    return theRoutines << "I was not able to find user: " << this->usernameUnsafe << ". The user does not exist or there is something wrong with the database";
  TimeWrapper currentTime;
  currentTime.AssignLocalTime();
  currentTime.ComputeTimeStringNonReadable();
  std::stringstream queryStream;
  queryStream << "UPDATE users SET user="
  << "'deleted" + CGI::StringToURLString(currentTime.theTimeStringNonReadable) << this->usernameSafe << "'"
  <<" WHERE user='" << this->usernameSafe << "'";
  DatabaseQuery renamingQuery(theRoutines, queryStream.str());
  return renamingQuery.flagQuerySucceeded;
}

bool UserCalculator::CreateMeIfUsernameUnique(DatabaseRoutines& theRoutines, bool recomputeSafeEntries)
{ MacroRegisterFunctionWithName("UserCalculator::CreateMeIfUsernameUnique");
  if (recomputeSafeEntries)
    this->ComputeSafeObjectNames();
  if (this->Iexist(theRoutines, false))
    return theRoutines << "User " << this->usernameUnsafe << " already exists. ";
  if (MathRoutines::StringBeginsWith(this->usernameSafe, "deleted"))
    return theRoutines << "You requested creation of user: " << this->usernameUnsafe
    << " however user names starting with 'deleted' are not allowed. ";
  std::stringstream queryStream;
  queryStream << "INSERT INTO calculatorUsers.users(user, email) VALUES('" << this->usernameSafe << "', '"
  << this->emailSafe << "')";
  DatabaseQuery theQuery(theRoutines, queryStream.str());
  return this->SetPassword(theRoutines, false, theRoutines.comments);
}

void UserCalculator::ComputeShaonedSaltedPassword(bool recomputeSafeEntries)
{ MacroRegisterFunctionWithName("UserCalculator::ComputeShaonedSaltedPassword");
  if (recomputeSafeEntries)
    this->ComputeSafeObjectNames();
  this->usernamePlusPassWord=this->usernameSafe;
  this->usernamePlusPassWord+=this->enteredPassword;
  this->enteredShaonedSaltedPassword=Crypto::computeSha1outputBase64(this->usernamePlusPassWord);
  //<-careful copying those around. We don't want to leave
  //any passwords in non-zeroed memory, even if properly freed.
  //Note the destructor of DatabaseRoutines zeroes some of the strings.
  //To do: make sure all crypto functions zero their buffers.
}

bool UserCalculator::SetColumnEntry
(const std::string& columnNameUnsafe, const std::string& theValueUnsafe, DatabaseRoutines& theRoutines, bool recomputeSafeEntries,
 std::stringstream* failureComments)
{ MacroRegisterFunctionWithName("UserCalculator::SetColumnEntry");
  if (recomputeSafeEntries)
    this->ComputeSafeObjectNames();
  if (this->currentTableSafe=="")
    crash << "Programming error: attempting to change column " << columnNameUnsafe << " without specifying a table. " << crash;
  std::string unused;
  std::string columnNameSafe = CGI::StringToURLString(columnNameUnsafe);
  std::string valueSafe= CGI::StringToURLString(theValueUnsafe);
  if (this->FetchOneColumn(columnNameUnsafe, unused, theRoutines, false, 0))
  { std::stringstream queryStream;
    queryStream << "UPDATE calculatorUsers.\"" << this->currentTableSafe << "\" SET \"" << columnNameSafe << "\"='"
    << valueSafe << "' WHERE user='" << this->usernameSafe << "'";
    //  stOutput << "Got to here: " << columnName << ". ";
    DatabaseQuery theDBQuery(theRoutines, queryStream.str(), failureComments);
//    stOutput << "<hr>Fired up query:<br>" << queryStream.str();
    if (!theDBQuery.flagQuerySucceeded)
    { if (failureComments!=0)
        *failureComments << "Failed update an already existing entry in column: " << columnNameUnsafe << ". ";
      stOutput << "Failed update an already existing entry in column: " << columnNameUnsafe << ". ";
      return false;
    }
  } else
  { std::stringstream queryStream;
    queryStream << "INSERT INTO calculatorUsers.\"" << this->currentTableSafe << "\"(user, \"" << columnNameSafe
    << "\") VALUES('" << this->usernameSafe << "', '" << valueSafe << "')";
    DatabaseQuery theDBQuery(theRoutines, queryStream.str());
    //stOutput << "<hr>Fired up query:<br>" << queryStream.str();
    if (!theDBQuery.flagQuerySucceeded)
    { if (failureComments!=0)
        *failureComments << "Failed to insert entry in table: " << this->currentTableUnsafe << ". ";
      stOutput << "Failed to insert entry in table: " << this->currentTableUnsafe << ". ";
      return false;
    }
  }
  return true;
}

bool UserCalculator::FetchOneUserRow
(DatabaseRoutines& theRoutines, std::stringstream& failureStream)
{ MacroRegisterFunctionWithName("UserCalculator::FetchOneUserRow");
  this->ComputeSafeObjectNames();
  if (this->currentTableUnsafe=="")
    crash << "Calling UserCalculator::FetchOneUserRow with an empty table is forbidden. " << crash;

  std::stringstream queryStream;
  queryStream << "SELECT * FROM calculatorUsers.\"" << this->currentTableSafe << "\" WHERE "
  << "user='" << this->usernameSafe << "'";
//  stOutput << "quering: " << queryStream.str();
  DatabaseQuery theQuery(theRoutines, queryStream.str(), &failureStream, 5);
  if (!theQuery.flagQuerySucceeded)
  { failureStream << "Query: " << queryStream.str() << " failed. ";
    return false;
  }
  if (!theQuery.flagQueryReturnedResult)
  { failureStream << "The table appears to be empty: query: " << queryStream.str() << " succeeded but returned no result. ";
    return false;
  }
  if (theQuery.allQueryResultStrings.size!=1)
  { failureStream << "Could not find entries for user " << this->usernameUnsafe << ".";
    return false;
  }
  this->selectedRowFieldsUnsafe.SetSize(theQuery.allQueryResultStrings[0].size);
  for (int i=0; i<this->selectedRowFieldsUnsafe.size; i++)
    this->selectedRowFieldsUnsafe[i]=CGI::URLStringToNormal(theQuery.allQueryResultStrings[0][i]);
  theQuery.close();
  std::stringstream queryStreamFields;
  queryStreamFields
  << "SELECT \"COLUMN_NAME\" FROM information_schema.COLUMNS WHERE "
  << "TABLE_SCHEMA='calculatorUsers' "
  << "AND TABLE_NAME='" << this->currentTableUnsafe << "' ";
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
  stOutput << "Fetched field names: " << this->selectedRowFieldNamesUnsafe.ToStringCommaDelimited()
  << " from: " << theFieldQuery.allQueryResultStrings.ToStringCommaDelimited()
  << ". All of those have values: "
  << this->selectedRowFieldsUnsafe.ToStringCommaDelimited()
  << " xtracted from " << theQuery.allQueryResultStrings.ToStringCommaDelimited();
  return true;
}

bool UserCalculator::ComputeSafeObjectNames()
{ MacroRegisterFunctionWithName("UserCalculator::ComputeSafeObjectNames");
  this->usernameSafe= CGI::StringToURLString(this->usernameUnsafe);
  this->currentTableSafe= CGI::StringToURLString(this->currentTableUnsafe);
  this->emailSafe= CGI::StringToURLString(this->emailUnsafe);
  this->activationTokenSafe=CGI::StringToURLString(this->activationTokenUnsafe);
  return true;
}

bool UserCalculator::SetPassword(DatabaseRoutines& theRoutines, bool recomputeSafeEntries, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::SetPassword");
  this->SetCurrentTable("users");
  if (recomputeSafeEntries)
    this->ComputeSafeObjectNames();
  if (this->enteredPassword=="")
  { commentsOnFailure << "Empty password not allowed. ";
    return false;
  }
//  stOutput << "Whats going on here<br>";
  this->ComputeShaonedSaltedPassword(false);
//  std::stringstream mustdeleteme;
  return this->SetColumnEntry("password", this->enteredShaonedSaltedPassword, theRoutines, false, &commentsOnFailure);
//  stOutput << mustdeleteme.str();
}

bool DatabaseRoutines::startMySQLDatabaseIfNotAlreadyStarted()
{ MacroRegisterFunctionWithName("DatabaseRoutines::startMySQLDatabaseIfNotAlreadyStarted");
  if (this->connection!=0)
    return true;
  return this->startMySQLDatabase();
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
  std::string selectAnsiQuotes="SET sql_mode='ANSI_QUOTES'";
  bool result= (mysql_query(this->connection, selectAnsiQuotes.c_str())==0);
  if (!result)
    return *this << "Command: <b>" << selectAnsiQuotes << "</b> failed. ";
  mysql_free_result( mysql_use_result(this->connection));
  return this->CreateTable("users", "\
    user VARCHAR(50) NOT NULL PRIMARY KEY,  \
    password VARCHAR(30) NOT NULL, \
    email VARCHAR(50) NOT NULL,\
    authenticationTokenCreationTime VARCHAR(50), \
    authenticationToken VARCHAR(50) , \
    activationToken VARCHAR(50)\
    ");
}

bool DatabaseRoutines::CreateTable
(const std::string& tableNameUnsafe, const std::string& desiredTableContent, std::stringstream* commentsOnCreation)
{ MacroRegisterFunctionWithName("DatabaseRoutines::CreateTable");
  if (tableNameUnsafe=="" )
  { if (commentsOnCreation!=0)
      *commentsOnCreation << "Empty table name not allowed.";
    return false;
  }
  if (this->TableExists(tableNameUnsafe))
    return true;
  std::string tableNameSafe=CGI::StringToURLString(tableNameUnsafe);
  std::string tableContent=desiredTableContent;
  if (tableContent=="")
    tableContent="user VARCHAR(50) NOT NULL PRIMARY KEY";
  tableContent="CREATE TABLE \"" + tableNameSafe+"\" (" + tableContent + ")";
  //CANNOT use object DatabaseQuery as that object invokes startMySQLDatabase
  //which in turn invokes the present function.
  if (commentsOnCreation!=0)
    *commentsOnCreation << "Proceeding to create table: " << tableNameUnsafe << ". ";
  bool result= (mysql_query(this->connection, tableContent.c_str())==0);
  if (!result)
    return *this << "Command:<br>" << tableContent << "<br>failed. ";
  mysql_free_result( mysql_use_result(this->connection));
  return result;
}

bool UserCalculator::IsAcceptableCharDatabaseInpuT(char theChar)
{ if (MathRoutines::isADigit(theChar))
    return true;
  if (MathRoutines::isALatinLetter(theChar))
    return true;
  switch (theChar)
  {
  case '.':
  case '@':
  case '+':
  case '/':
  case '%':
  case '=':
  case ' ':
    return true;
  default:
    return false;
  }
}

bool UserCalculator::IsAcceptableDatabaseInpuT(const std::string& input, std::stringstream* comments)
{ MacroRegisterFunctionWithName("UserCalculator::IsAcceptableDatabaseInput");
  for (unsigned i=0; i<input.size(); i++)
    if (!UserCalculator::IsAcceptableCharDatabaseInpuT(input[i]))
    { if (comments!=0)
        *comments << "Input: " << input << " contains at least one invalid character: " << input[i] << ".";
      return false;
    }
  return true;
}

bool DatabaseRoutines::TableExists(const std::string& tableNameUnsafe)
{ MacroRegisterFunctionWithName("DatabaseRoutines::TableExists");
  std::string tableNameSafe=CGI::StringToURLString(tableNameUnsafe);
  std::stringstream queryStream;
  queryStream << "SELECT 1 FROM " << this->theDatabaseName << ".\"" << tableNameSafe << "\"";
  bool result=(mysql_query(this->connection, queryStream.str().c_str())==0);
  *this << "Executed query to check table existence: " << queryStream.str() << ". ";
//  stOutput << "Executed query: " << queryStream.str() << "<br>";
  mysql_free_result( mysql_use_result(this->connection));
  return result;
}

bool DatabaseRoutines::innerTestLogin(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerTestLogin");
  UserCalculator theUser;
  if (!theUser.getUserAndPass(theCommands, input))
    return false;
  DatabaseRoutines theRoutines;
  if (!theUser.TryToLogIn(theRoutines))
    return output.MakeError
    ( "Failed to login, username: " + theUser.usernameUnsafe+ " password: " + theUser.enteredPassword +
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

std::string UserCalculator::GetSelectedRowEntry(const std::string& theKey)
{ MacroRegisterFunctionWithName("UserCalculator::GetSelectedRowEntry");
  int theIndex= this->selectedRowFieldNamesUnsafe.GetIndex(theKey);
  if (theIndex==-1)
    return "";
  return this->selectedRowFieldsUnsafe[theIndex];
}

bool DatabaseRoutines::ExtractEmailList(const std::string& emailList, List<std::string>& outputList, std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::ExtractEmailList");
  List<char> delimiters;
  delimiters.AddOnTop(' ');
  delimiters.AddOnTop('\r');
  delimiters.AddOnTop('\n');
  delimiters.AddOnTop('\t');
  delimiters.AddOnTop(',');
  delimiters.AddOnTop(';');
  MathRoutines::StringSplitExcludeDelimiters(emailList, delimiters, outputList);
  return true;
}

bool DatabaseRoutines::SendActivationEmail(const std::string& emailList, std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::SendActivationEmail");
  List<std::string> theEmails;
  this->ExtractEmailList(emailList, theEmails, comments);
  return this->SendActivationEmail(theEmails, true, comments);
}

void UserCalculator::ComputeActivationToken()
{ MacroRegisterFunctionWithName("UserCalculator::ComputeActivationToken");
  TimeWrapper now;
  //stOutput << "Resetting authentication token. ";
  now.AssignLocalTime();
  std::stringstream activationTokenStream;
  activationTokenStream << now.theTimeStringNonReadable << rand();
  this->activationTokenUnsafe=Crypto::computeSha1outputBase64(activationTokenStream.str());
}

bool DatabaseRoutines::SendActivationEmail(const List<std::string>& theEmails, bool forceResend, std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::SendActivationEmail");
  if (!this->ColumnExists("activationToken", "users", comments))
    if (!this->CreateColumn("activationToken", "users", comments))
      return false;
  UserCalculator currentUser;
  currentUser.SetCurrentTable("users");
  bool result;
  for (int i=0; i<theEmails.size; i++)
  { currentUser.usernameUnsafe=theEmails[i];
    currentUser.emailUnsafe=theEmails[i];
    currentUser.ComputeActivationToken();
    if (!currentUser.SetColumnEntry("activationToken", currentUser.activationTokenUnsafe, *this, true, &comments))
    { comments << "Setting activation token failed.";
      return false;
    }
    if (!currentUser.SendActivationEmail(*this, comments))
    { comments << "Failed to send activation email to: " << currentUser.usernameUnsafe;
      result=false;
      continue;
    }
  }
  return result;
}

bool DatabaseRoutines::AddUsersFromEmails
(const std::string& emailList, std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::AddUsersFromEmails");
  List<std::string> theEmails;
  this->ExtractEmailList(emailList, theEmails, comments);
  stOutput << " <br>creating users: " << theEmails.ToStringCommaDelimited()
  << "<br>";
  UserCalculator currentUser;
  bool result=true;
  for (int i=0; i<theEmails.size; i++)
  { currentUser.usernameUnsafe=theEmails[i];
    currentUser.emailUnsafe=theEmails[i];
    if (!currentUser.Iexist(*this, true))
    { if (!currentUser.CreateMeIfUsernameUnique(*this, false))
      { comments << "Failed to create user: " << currentUser.usernameUnsafe;
        result=false;
        continue;
      }
      currentUser.SetColumnEntry("email", theEmails[i], *this, false, &comments);
    }
  }
  if (!result )
  { comments << "<br>Failed to create all users. Additional comments: " << this->comments.str();
    return result;
  }
  return this->SendActivationEmail(theEmails, false, comments);
}

bool UserCalculator::SendActivationEmail(DatabaseRoutines& theRoutines, std::stringstream& comments)
{ MacroRegisterFunctionWithName("UserCalculator::SendActivationEmail");
  if (!this->FetchOneUserRow(theRoutines, comments))
    return false;
  stOutput << "<hr> all result strings: " << this->selectedRowFieldNamesUnsafe.ToStringCommaDelimited();
  stOutput << "<br> all result string names: " << this->selectedRowFieldsUnsafe.ToStringCommaDelimited();
  this->activationTokenUnsafe= this->GetSelectedRowEntry("activationToken");
  this->emailUnsafe=this->GetSelectedRowEntry("email");
  if (this->activationTokenUnsafe=="")
  { comments << "Failed to fetch activation token";
    return false;
  }
  if (this->activationTokenUnsafe=="activated")
  { comments << "Account already activated";
    return false;
  }
  EmailRoutines theEmailRoutines;
  theEmailRoutines.toEmail=this->emailUnsafe;
  theEmailRoutines.subject="NO REPLY: Activation of a Math homework account. ";
  std::stringstream emailStream;
  emailStream << "Dear student,\nthis is an automated email sent with an activation token for your "
  << " math homework account. To activate your account and set up your password, please follow the link below. "
  << " If you have any technical problems or have questions, please DO NOT HIT the REPLY BUTTON; "
  << " instead, post your quesiton on piazza.com or email your question to "
  << " todor.milev@gmail.com\n\n";
  emailStream << this->activationTokenUnsafe << "\n\nGood luck with our course, \n Your calculus instructors.";
  std::string emailLog=theGlobalVariables.CallSystemWithOutput(theEmailRoutines.GetCommandToSendEmailWithMailX());
  stOutput << "Calling system with:<br>" << theEmailRoutines.GetCommandToSendEmailWithMailX()
  << "<br>\n to get output: \n<br>" << emailLog;
  comments << "\nEmail sending log:\n" << emailLog;
  return true;
}

bool DatabaseRoutines::innerAddUsersFromEmailListAndCourseName(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerAddUsersFromEmailListAndCourseName");
  if (input.children.size!=3)
    return theCommands << "addUsers takes as input two arguments.";

  std::string inputEmailList, inputClassHome;
  if (!input[1].IsOfType(&inputEmailList))
    return theCommands << "First argument of " << input.ToString() << " is not a string. ";
  if (!input[2].IsOfType(&inputClassHome))
    return theCommands << "Second argument of " << input.ToString() << " is not a string. ";
  crash << "not implemented yet" << crash;
  return false;
/*  DatabaseRoutines theRoutines;
  if (!theRoutines.AddUsersFromEmails(inputEmailList, inputClassHome, theCommands.Comments))
    return false;
  std::stringstream out;
  out << "Successfully added students.";
  return output.AssignValue(out.str(), theCommands);*/
}

bool DatabaseRoutines::innerSendActivationEmailUsers(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerAddUsersFromEmailList");
  std::string inputEmailList;
  if (!input.IsOfType(&inputEmailList))
    return theCommands << "Argument " << input.ToString() << " is not a string. ";
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return theCommands << "Adding users requires admin rights. ";
  DatabaseRoutines theRoutines;
  if (!theRoutines.SendActivationEmail(inputEmailList, theCommands.Comments))
    return false;
  std::stringstream out;
  out << "Successfully added students. " ;
  return output.AssignValue(out.str(), theCommands);
}

bool DatabaseRoutines::innerAddUsersFromEmailList(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerAddUsersFromEmailList");
  std::string inputEmailList;
  if (!input.IsOfType(&inputEmailList))
    return theCommands << "Argument " << input.ToString() << " is not a string. ";
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return theCommands << "Adding users requires admin rights. ";
  DatabaseRoutines theRoutines;
  if (!theRoutines.AddUsersFromEmails(inputEmailList, theCommands.Comments))
    return false;
  std::stringstream out;
  out << "Successfully added students. " ;
  return output.AssignValue(out.str(), theCommands);
}

bool DatabaseRoutines::innerAddUser(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerAddUser");
  UserCalculator theUser;
  if (!theUser.getUserPassAndEmail(theCommands, input))
    return false;
  if (MathRoutines::StringBeginsWith(theUser.usernameUnsafe, "deleted"))
    return output.MakeError("User names starting with 'deleted' are not allowed.", theCommands);
  DatabaseRoutines theRoutines;
  if (theGlobalVariables.userDefault!=theGlobalVariables.userCalculatorAdmin)
  { UserCalculator adminUser;
    adminUser.usernameUnsafe=theGlobalVariables.userCalculatorAdmin;
    if (adminUser.Iexist(theRoutines, true))
      return output.MakeError("Only calculator admin is allowed to add users", theCommands);
  }
  //reaching this piece of code means the calculator has no admin user, so the current user assumes admin powers.
  theUser.CreateMeIfUsernameUnique(theRoutines, true);
  return output.AssignValue(theRoutines.comments.str(), theCommands);
}

bool DatabaseRoutines::innerDeleteUser(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerDeleteUser");
  if (theGlobalVariables.userCalculatorAdmin!=theGlobalVariables.userDefault)
    return output.MakeError("Deleting users allowed only as logged-in admin. ", theCommands);
  UserCalculator theUser;
  if (!theUser.getUser(theCommands, input))
    return false;
  DatabaseRoutines theRoutines;
  theUser.DeleteMe(theRoutines, true);
  return output.AssignValue(theRoutines.comments.str(), theCommands);
}

bool DatabaseRoutines::innerSetUserPassword(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerSetUserPassword");
  if (!theGlobalVariables.flagLoggedIn)
    return output.MakeError
    ("Function innerSetUserPassword can be invoked only for logged in users. ", theCommands);
  UserCalculator theUser;
  if (!theUser.getUserAndPass(theCommands, input))
    return false;
  bool authorized=false;
  if (theUser.usernameUnsafe==theGlobalVariables.userDefault)
    authorized=true;
  else if (theGlobalVariables.userDefault==theGlobalVariables.userCalculatorAdmin)
  { authorized=true;
    theCommands << "Password change: invoking admin powers...";
  }
  if (!authorized)
    return theCommands << "Password change failed for user " << theUser.usernameUnsafe
    << ": not enough privileges. ";
  DatabaseRoutines theRoutines;
  theUser.SetPassword(theRoutines, true, theCommands.Comments);
  return output.AssignValue(theRoutines.comments.str(), theCommands);
}

bool UserCalculator::getUser(Calculator& theCommands, const Expression& input)
{ MacroRegisterFunctionWithName("UserCalculator::getUser");
  if (!input.IsOfType<std::string>(&this->usernameUnsafe))
  { theCommands << "<hr>Argument " << input.ToString() << " is supposed to be a string.";
    this->usernameUnsafe=input.ToString();
  }
  return true;
}

bool UserCalculator::getUserAndPass(Calculator& theCommands, const Expression& input)
{ MacroRegisterFunctionWithName("UserCalculator::getUserAndPass");
  if (input.children.size!=3)
    return theCommands << "UserCalculator::getUserAndPass takes as input 2 arguments (user name and password). ";
  if (!input[1].IsOfType<std::string>(&this->usernameUnsafe))
  { theCommands << "<hr>Argument " << input[1].ToString() << " is supposed to be a string. ";
    this->usernameUnsafe=input[1].ToString();
  }
  if (!input[2].IsOfType<std::string>(&this->enteredPassword))
  { theCommands << "<hr>Argument " << input[2].ToString() << " is supposed to be a string. ";
    this->enteredPassword=input[2].ToString();
    this->enteredAuthenticationTokenUnsafe=this->enteredPassword;
  }
  return true;
}

bool UserCalculator::getUserPassAndSelectedColumns(Calculator& theCommands, const Expression& input)
{ MacroRegisterFunctionWithName("UserCalculator::getUserPassAndSelectedColumns");
  if (input.children.size<3)
    return theCommands << "UserCalculator::getUserPassAndSelectedColumns takes as input at least 2 arguments (user name and password). ";
  bool result = this->getUserPassAndExtraData(theCommands, input, this->selectedColumnsUnsafe);
  this->selectedColumnValuesUnsafe.initFillInObject(this->selectedColumnsUnsafe.size, "");
  this->selectedColumnsRetrievalFailureRemarks.initFillInObject(this->selectedColumnsUnsafe.size, "");
  return result;
}

bool UserCalculator::getUserPassAndExtraData(Calculator& theCommands, const Expression& input, List<std::string>& outputData)
{ MacroRegisterFunctionWithName("UserCalculator::getUserPassAndExtraData");
  if (input.children.size<4)
    return theCommands << "UserCalculator::getUserPassAndExtraData takes as input at least 2 arguments (user name and password). ";
  if (!input[1].IsOfType<std::string>(&this->usernameUnsafe))
  { theCommands << "<hr>Argument " << input[1].ToString() << " is supposed to be a string.";
    this->usernameUnsafe=input[1].ToString();
  }
  if (!input[2].IsOfType<std::string>(&this->enteredPassword))
  { theCommands << "<hr>Argument " << input[2].ToString() << " is supposed to be a string.";
    this->enteredPassword=input[2].ToString();
    this->enteredAuthenticationTokenUnsafe=this->enteredPassword;
  }
  outputData.SetSize(input.children.size-3);
  for (int i=3; i<input.children.size; i++ )
    if (!input[i].IsOfType<std::string>(& outputData[i-3]))
    { theCommands << "<hr>Argument " << input[i].ToString() << " is supposed to be a string";
      outputData[i-3]=input[i].ToString();
    }
  return true;
}

bool UserCalculator::getUserPassAndEmail(Calculator& theCommands, const Expression& input)
{ MacroRegisterFunctionWithName("UserCalculator::getUserPassAndEmail");
  if (input.children.size!=4)
    return theCommands << "UserCalculator::getUserPassAndEmail takes as input 3 arguments (user name, email and password). ";
  if (!input[1].IsOfType<std::string>(&this->usernameUnsafe))
  { theCommands << "<hr>Argument " << input[1].ToString() << " is supposed to be a string.";
    this->usernameUnsafe=input[1].ToString();
  }
  if (!input[2].IsOfType<std::string>(&this->enteredPassword))
  { theCommands << "<hr>Argument " << input[2].ToString() << " is supposed to be a string.";
    this->enteredPassword=input[2].ToString();
    this->enteredAuthenticationTokenUnsafe=this->enteredPassword;
  }
  if (!input[3].IsOfType<std::string>(&this->emailUnsafe))
  { theCommands << "<hr>Argument " << input[3].ToString() << " is supposed to be a string.";
    this->emailUnsafe=input[3].ToString();
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

void UserCalculator::FetchColumns(DatabaseRoutines& theRoutines, bool recomputeSafeEntries)
{ MacroRegisterFunctionWithName("UserCalculator::FetchColumns");
  if (recomputeSafeEntries)
    this->ComputeSafeObjectNames();
  if (this->selectedColumnsUnsafe.size!=this->selectedColumnValuesUnsafe.size ||
      this->selectedColumnsUnsafe.size!=this->selectedColumnsRetrievalFailureRemarks.size)
    crash << "This is a programming error: selected columns not initialized properly" << crash;
  for (int i=0; i<this->selectedColumnsUnsafe.size; i++)
  { std::stringstream tempStream;
    this->FetchOneColumn
    (this->selectedColumnsUnsafe[i], this->selectedColumnValuesUnsafe[i], theRoutines, false, &tempStream);
    this->selectedColumnsRetrievalFailureRemarks[i]=tempStream.str();
  }
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
  if (theUser.usernameUnsafe!=theGlobalVariables.userCalculatorAdmin)
    return output.AssignValue
    ((std::string)"At the moment, the GetUserDetails function is implemented only for the admin user. ", theCommands);
  if (!theUser.Authenticate(theRoutines, true))
    return theCommands << "Authentication failed. " << theRoutines.comments.str();
  return output.AssignValue(theRoutines.ToStringAllUsersHTMLFormat(), theCommands);
}

bool DatabaseRoutines::innerDisplayTables(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerDisplayTables");
  std::stringstream out;
  DatabaseRoutines theRoutines;
  List<std::string> theTables;
  if (!theRoutines.FetchTableNames(theTables, theCommands.Comments))
    return false;
  out << "<table>";
  for (int i=0; i<theTables.size; i++)
    out << "<tr><td>" << theTables[i] << "</td></tr>";
  out << "</table>";
  return output.AssignValue(out.str(), theCommands);
}

bool DatabaseRoutines::innerDisplayDatabaseTable(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerDisplayDatabaseTable");
  if (!theGlobalVariables.flagLoggedIn || theGlobalVariables.userDefault!=theGlobalVariables.userCalculatorAdmin)
    return theCommands << "Displaying database tables allowed only for logged-in admins. ";
  std::string desiredTableName;
  if (!input.IsOfType<std::string>(&desiredTableName))
  { theCommands << "<hr>Argument " << input.ToString() << " is supposed to be a string. ";
    desiredTableName=input.ToString();
  }
  DatabaseRoutines theRoutines;
  return output.AssignValue(theRoutines.ToStringTable(desiredTableName), theCommands);
}

bool DatabaseRoutines::innerGetUserDBEntry(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerGetAuthenticationTokenCreationTime");
  UserCalculator theUser;
  if (!theUser.getUserPassAndSelectedColumns(theCommands, input))
    return false;
  DatabaseRoutines theRoutines;
  if (!theUser.Authenticate(theRoutines, true))
    return theCommands << theRoutines.comments.str();
  theUser.FetchColumns(theRoutines, true);
  return output.AssignValue(theUser.ToStringSelectedColumns(), theCommands);
}

bool DatabaseRoutines::innerGetAuthentication(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerGetAuthentication");
  UserCalculator theUser;
  if (!theUser.getUserAndPass(theCommands, input))
    return false;
  DatabaseRoutines theRoutines;
  if (!theUser.Authenticate(theRoutines, true))
    return output.MakeError("Failed to authenticate. ", theCommands);
  return output.AssignValue(theUser.actualAuthenticationTokeNUnsafe, theCommands);
}

bool DatabaseRoutines::innerCreateTeachingClass(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerCreateTeachingClass");
  UserCalculator theUser;
  List<std::string> className;
  if (!theUser.getUserPassAndExtraData(theCommands, input, className))
    return false;
  DatabaseRoutines theRoutines;
  if (!theUser.Authenticate(theRoutines, true))
    return output.MakeError("Failed to authenticate. ", theCommands);
  if (!theRoutines.CreateTable(className[0],
  "user VARCHAR(50) NOT NULL PRIMARY KEY, \
  VARCHAR(50) NOT NULL", &theCommands.Comments))
    return output.AssignValue(theRoutines.comments.str(), theCommands);
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

