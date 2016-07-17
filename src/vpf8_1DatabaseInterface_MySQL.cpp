//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader7DatabaseInterface_MySQL.h"
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
    }

    return true;
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
  { if (!theUser.Iexist(theRoutines))
    { if (comments!=0)
        *comments << "<b>First login of user admin: setting admin pass. </b>";
      theUser.CreateMeIfUsernameUnique(theRoutines, comments);
      theUser.SetColumnEntry("activationToken", "activated", theRoutines, comments);
      theUser.SetColumnEntry("userRole", "admin", theRoutines, comments);
      outputUserRole="admin";
    }
    return true;
  }
  if (comments!=0)
    *comments << DatabaseRoutines::ToStringSuggestionsReasonsForFailure(inputUsernameUnsafe, theRoutines, theUser);
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
  theUser.username=theGlobalVariables.userDefault;
  theUser.currentTable="users";
  theUser.ResetAuthenticationToken(theRoutines, 0);
  theGlobalVariables.SetWebInpuT("authenticationToken", "");
  return true;
#else
  return true;
#endif
}

bool DatabaseRoutinesGlobalFunctions::SetPassword
(const std::string& inputUsername, const std::string& inputNewPassword, std::string& outputAuthenticationToken,
 std::stringstream& comments)
{
  (void) inputUsername; (void) inputNewPassword; (void) outputAuthenticationToken; (void) comments;
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::SetPassword");
  if(!theGlobalVariables.flagLoggedIn)
  { comments << "Changing passwords allowed for logged-in users only. ";
    return false;
  }
  DatabaseRoutines theRoutines;
  UserCalculator theUser;
  theUser.username=inputUsername;
  theUser.enteredPassword=inputNewPassword;
  bool result= theUser.SetPassword(theRoutines, &comments);
  theUser.ResetAuthenticationToken(theRoutines, &comments);
  outputAuthenticationToken= theUser.actualAuthenticationToken.value;
  return result;
#else
  return false;
#endif // MACRO_use_MySQL
}

bool DatabaseRoutinesGlobalFunctions::RowExists
  (const std::string& inputUsername, const std::string& tableName, std::stringstream& comments)
{ (void) inputUsername; (void) tableName; (void) comments;
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::RowExists");
  if (!theGlobalVariables.flagLoggedIn)
    return false;
  DatabaseRoutines theRoutines;
  return theRoutines.RowExists((std::string) "username", inputUsername, tableName, &comments);
#else
  return false;
#endif // MACRO_use_MySQL
}

bool DatabaseRoutinesGlobalFunctions::SetEntry
  (const std::string& inputUsername, const std::string& tableNameUnsafe, const std::string& keyNameUnsafe,
   const std::string& valueUnsafe, std::stringstream& comments)
{ (void) inputUsername; (void) tableNameUnsafe; (void) keyNameUnsafe; (void) valueUnsafe; (void) comments;
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::SetEntry");
  if (!theGlobalVariables.flagLoggedIn)
    return false;
  DatabaseRoutines theRoutines;
  UserCalculator theUser;
  theUser.username=inputUsername;
//  stOutput << "setting table name to: " << tableName << "<br>";
  theUser.currentTable=tableNameUnsafe;
  return theUser.SetColumnEntry(keyNameUnsafe, valueUnsafe, theRoutines, &comments);
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
{ (void) tableName; (void) comments;
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::CreateTable");
  if (!theGlobalVariables.flagLoggedIn)
    return false;
  DatabaseRoutines theRoutines;
  theRoutines.startMySQLDatabaseIfNotAlreadyStarted(&comments);
  return theRoutines.CreateTable(tableName,  "username VARCHAR(255) NOT NULL PRIMARY KEY  ", &comments, 0);
#else
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctions::ColumnExists
(const std::string& columnNameUnsafe, const std::string& tableNameUnsafe, std::stringstream& commentsStream)
{ (void) columnNameUnsafe; (void) tableNameUnsafe; (void) commentsStream;
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
{ (void) columnNameUnsafe; (void) tableNameUnsafe; (void) commentsOnCreation;
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
{ (void) tableName; (void) comments;
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::TableExists");
  if (!theGlobalVariables.flagLoggedIn)
    return false;
  DatabaseRoutines theRoutines;
  if (!theRoutines.startMySQLDatabaseIfNotAlreadyStarted(&comments))
    comments << "Failed to start database. ";
  return theRoutines.TableExists(tableName, &comments);
#else
  return true;
#endif
}

bool DatabaseRoutinesGlobalFunctions::FetchTablE
(List<List<std::string> >& output,
 List<std::string>& outputColumnLabels,
 bool& outputWasTruncated, int& actualNumRowsIfTruncated,
 const std::string& tableName, std::stringstream& comments)
{ (void) output; (void) outputColumnLabels; (void) outputWasTruncated; (void) actualNumRowsIfTruncated; (void) tableName; (void) comments;
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::FetchTable");
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  return theRoutines.FetchTablE
  (output, outputColumnLabels, outputWasTruncated, actualNumRowsIfTruncated,
   tableName, comments);
#else
  comments << "Database not available.";
  return false;
#endif // MACRO_use_MySQL
}

bool DatabaseRoutinesGlobalFunctions::FetchTableFromDatabaseIdentifier
(List<List<std::string> >& output,
 List<std::string>& outputColumnLabels,
 bool& outputWasTruncated, int& actualNumRowsIfTruncated,
 const std::string& tableIdentifier, std::stringstream& comments)
{ (void) output; (void) outputColumnLabels; (void) outputWasTruncated; (void) actualNumRowsIfTruncated; (void) tableIdentifier; (void) comments;
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::FetchTable");
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  return theRoutines.FetchTableFromDatabaseIdentifier
  (output, outputColumnLabels, outputWasTruncated, actualNumRowsIfTruncated,
   tableIdentifier, comments);
#else
  comments << "Database not available.";
  return false;
#endif // MACRO_use_MySQL
}

bool DatabaseRoutinesGlobalFunctions::FetchEntry
(const std::string& inputUsername, const std::string& tableName, const std::string& keyName,
 std::string& output, std::stringstream& comments)
{ (void) inputUsername; (void) tableName; (void) keyName; (void) output; (void) comments;
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::FetchEntry");
  if (!theGlobalVariables.flagLoggedIn)
    return false;
  DatabaseRoutines theRoutines;
  UserCalculator theUser;
  theUser.username=inputUsername;
  theUser.currentTable=tableName;
  return theUser.FetchOneColumn(keyName, output, theRoutines, &comments);
#else
  return true;
#endif
}

ProblemData::ProblemData()
{ this->randomSeed=0;
  this->flagRandomSeedComputed=false;
  this->numCorrectlyAnswered=0;
  this->totalNumSubmissions=0;
  this->flagProblemWeightIsOK=false;
}

void ProblemData::AddEmptyAnswerIdOnTop(const std::string& inputAnswerId)
{ Answer theAnswer;
  theAnswer.answerId=inputAnswerId;
  this->theAnswers.AddOnTop(theAnswer);
}

std::string ProblemData::ToStringAvailableAnswerIds()
{ std::stringstream out;
  out << "Available answer ids: ";
  for (int i=0; i<this->theAnswers.size; i++)
  { out << this->theAnswers[i].answerId;
    if (i!=this->theAnswers.size-1)
      out << ", ";
  }
  return out.str();
}

std::string ProblemData::ToString()
{ std::stringstream out;
  out << "Problem data. "
  << "Random seed: " << this->randomSeed;
  if (this->flagRandomSeedComputed)
    out << " (loaded from database)";
  out << ". ";
  for (int i=0; i<this->theAnswers.size; i++)
  { Answer& currentA=this->theAnswers[i];
    out << "AnswerId: " << currentA.answerId;
    out << ", numCorrectSubmissions: " << currentA.numCorrectSubmissions;
    out << ", numSubmissions: " << currentA.numSubmissions;
    out << ", firstCorrectAnswer: ";
    if (currentA.firstCorrectAnswerClean!="")
      out << "none yet";
    else
      out << currentA.firstCorrectAnswerClean;
  }
  return out.str();
}

#ifdef MACRO_use_MySQL
#include "vpfHeader5Crypto.h"
#include <time.h>
#include <ctime>

std::string MySQLdata::GetDatA()const
{ return "'" + CGI::StringToURLString(this->value) + "'";
}

std::string MySQLdata::GetIdentifierNoQuotes()const
{ std::string result=CGI::StringToURLString(this->value);
  if (result.size()<=30)
    return result;
  return result.substr(0,30)+ Crypto::computeSha1outputBase64(this->value);
}

std::string MySQLdata::GetIdentifieR()const
{ MacroRegisterFunctionWithName("MySQLdata::GetIdentifieR");
  return "`"+ this->GetIdentifierNoQuotes()+"`";
}

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

bool DatabaseRoutines::CreateColumn
(const std::string& columnNameUnsafe, const std::string& tableNameUnsafe,
 std::stringstream& commentsOnCreation)
{ MacroRegisterFunctionWithName("DatabaseRoutines::CreateColumn");
//  stOutput << "Creating column pt 1.";
  this->startMySQLDatabaseIfNotAlreadyStarted(&commentsOnCreation);
  if (!this->TableExists(tableNameUnsafe, &commentsOnCreation))
    return false;
//  stOutput << "Creating column pt 2.";
  MySQLdata columnName=columnNameUnsafe;
  MySQLdata tableName=tableNameUnsafe;
//  stOutput << "<hr>BEFORE alter command";
  std::stringstream commandStream;
  commandStream << "ALTER TABLE " << tableName.GetIdentifieR()
  << " ADD " << columnName.GetIdentifieR() << " LONGTEXT";
//  stOutput << "<br>got to here<br>hrbr<br>";
  DatabaseQuery theQuery(*this, commandStream.str(), &commentsOnCreation);
//  stOutput << "<br>AFTER alter command: " << theRoutines.comments.str() << "<hr>";
  if (!theQuery.flagQuerySucceeded)
    commentsOnCreation << "Failed to create column. ";
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

std::string UserCalculator::ToString()
{ MacroRegisterFunctionWithName("UserCalculator::ToString");
  std::stringstream out;
  out << "Calculator user: " << this->username.value;
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

std::string DatabaseRoutines::ToStringTablE(const MySQLdata& inputTable)
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringTablE");
  return this->ToStringTablE(inputTable.GetIdentifierNoQuotes());
}

std::string DatabaseRoutines::ToStringTableFromTableIdentifier(const std::string& tableIdentifier)
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringTable");
  std::stringstream out, comments;
  List<List<std::string> > theTable;
  List<std::string> columnLabels;
  bool wasTruncated=false;
  int actualNumEntries=-1;
  if (!this->FetchTableFromDatabaseIdentifier
      (theTable, columnLabels, wasTruncated, actualNumEntries, tableIdentifier, comments))
  { out << "<b>Failed to fetch table: " << tableIdentifier << ".</b> Comments: " << comments.str();
    return out.str();
  }
  out << "Table has " << actualNumEntries << " entries. ";
  if (wasTruncated)
    out << "<b>The number of entries was truncated to " << theTable.size << ". <b>";
  out << "<table><tr>";
  for (int i=0; i<columnLabels.size; i++)
    out << "<td>" << columnLabels[i] << "</td>";
  out << "</tr>";
  for (int i=0; i<theTable.size; i++)
  { out << "<tr>";
    for (int j=0; j<theTable[i].size; j++)
      out << "<td>" << CGI::URLKeyValuePairsToNormalRecursiveHtml( theTable[i][j]) << "</td>";
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
    linkStream << theGlobalVariables.DisplayNameExecutableWithPath
    << "?request=browseDatabase&currentDatabaseTable="
    << CGI::StringToURLString(tableNames[i]) << "&" << theGlobalVariables.ToStringCalcArgsNoNavigation();
    out << "<tr><td><a href=\"" << linkStream.str() << "\">" << CGI::URLStringToNormal(tableNames[i])
    << "</a></td></tr>";
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
  if (currentTable.find("`")!=std::string::npos)
  { std::stringstream out;
    out << "<b>The table identifier: " << currentTable << " contains the ` character and is invalid. </b>";
    return out.str();
  }
  if (currentTable=="")
    return this->ToStringAllTables();
  if (currentTable.size()>=64)
  { std::stringstream out;
    out << "<b>The table identifier: " << currentTable << " is longer than 64 characters and is invalid. </b>";
    return out.str();
  }
  return this->ToStringTableFromTableIdentifier(currentTable);
}

bool DatabaseRoutines::FetchTablE
(List<List<std::string> >& output,
 List<std::string>& outputColumnLabels,
 bool& outputWasTruncated, int& actualNumRowsIfTruncated,
 const MySQLdata& inputTable, std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::FetchTablE");
  return this->FetchTableFromDatabaseIdentifier
  (output, outputColumnLabels, outputWasTruncated, actualNumRowsIfTruncated,
   inputTable.GetIdentifierNoQuotes(), comments);
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

DatabaseRoutines::DatabaseRoutines()
{ this->connection=0;
  this->MaxNumRowsToFetch=1000;
  this->flagFirstLogin=false;
}

DatabaseRoutines::~DatabaseRoutines()
{ if (this->connection!=0)
    mysql_close(this->connection);   // Close and shutdown
  this->connection=0;
}

DatabaseQuery::DatabaseQuery(DatabaseRoutines& inputParent, const std::string& inputQuery, std::stringstream* outputFailureComments, int inputMaxNumRowsToRead)
{ MacroRegisterFunctionWithName("DatabaseQuery::DatabaseQuery");
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
    if (!this->parent->startMySQLDatabase(outputFailureComments, 0))
    { if (outputFailureComments!=0)
        *outputFailureComments << "Failed to start database. ";
      return;
    }
//  stOutput << "got to here in the queery query part -1";
  int queryError=mysql_query(this->parent->connection, this->theQueryString.c_str());
  if (queryError!=0)
  { if (outputFailureComments!=0)
    { *outputFailureComments << "Query<br>" << this->theQueryString << "<br>failed. ";
      *outputFailureComments << mysql_error(this->parent->connection);
    }
    return;
  }
  this->flagQuerySucceeded=true;
  this->theQueryResult= mysql_store_result(this->parent->connection);
  //stOutput << "and even to here";
  if (this->theQueryResult==0)
  { //if (this->failurecomments!=0)
    //  *this->failurecomments << "Query succeeded. ";
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
    { if (outputFailureComments!=0)
        *outputFailureComments << "Query result row " << i+1 << " did not return properly. " ;
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

bool UserCalculator::FetchOneColumn
(const std::string& columnNameUnsafe, std::string& outputUnsafe,
 DatabaseRoutines& theRoutines, std::stringstream* failureComments)
{ MacroRegisterFunctionWithName("UserCalculator::FetchOneColumn");
  return theRoutines.FetchEntry
  ((std::string) "username", this->username, this->currentTable,
   columnNameUnsafe, outputUnsafe, failureComments);
}

bool UserCalculator::AuthenticateWithToken(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::AuthenticateWithToken");
  this->currentTable="users";
  if (this->enteredAuthenticationToken=="")
    return false;
  std::string authenticationCreationTimeSTRING;
  if (!this->FetchOneColumn
      ("authenticationCreationTime", authenticationCreationTimeSTRING,
       theRoutines, commentsOnFailure))
  { if (commentsOnFailure!=0)
      *commentsOnFailure << "Failed to fetch authentication token creation time: "
      << commentsOnFailure;
    return false;
  }
  this->authenticationCreationTime=authenticationCreationTimeSTRING;
  TimeWrapper now;
  now.AssignLocalTime();
  this->approximateHoursSinceLastTokenWasIssued=now.SubtractAnotherTimeFromMeAndGet_APPROXIMATE_ResultInHours
  (this->authenticationCreationTime);
//  stOutput << "approx hours since token issued: " << this->approximateHoursSinceLastTokenWasIssued;
//  if (this->approximateHoursSinceLastTokenWasIssued>3600 || this->approximateHoursSinceLastTokenWasIssued<=0) //3600 hours = 150 days, a bit more than the length of a semester
//    return false;
  if (!this->FetchOneColumn
      ("authenticationToken", this->actualAuthenticationToken.value,
      theRoutines, commentsOnFailure))
    return false;
  return this->enteredAuthenticationToken.value==this->actualAuthenticationToken.value;
}

bool UserCalculator::Authenticate(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  this->currentTable="users";
  if (!this->Iexist(theRoutines))
  { //if (this->username!="")
    //  stOutput << "user: '" << this->username << "' does not exist";
    if (commentsOnFailure!=0)
      *commentsOnFailure << "User " << this->username.value << " does not exist. ";
    return false;
  } else
  { std::string userNameAsStoredInDB;
    if (!this->FetchOneColumn("username", userNameAsStoredInDB, theRoutines, commentsOnFailure))
      return false;
    this->username=userNameAsStoredInDB;
  }
  if (this->AuthenticateWithToken(theRoutines, commentsOnFailure))
  { if(!this->FetchOneColumn("userRole", this->userRole, theRoutines, commentsOnFailure))
    { if (commentsOnFailure!=0)
        *commentsOnFailure << "Failed to fetch user role";
      return false;
    }
    return true;
  }
  bool result= this->AuthenticateWithUserNameAndPass(theRoutines, commentsOnFailure);
  if (result)
    if(!this->FetchOneColumn("userRole", this->userRole, theRoutines, commentsOnFailure))
    { if (commentsOnFailure!=0)
        *commentsOnFailure << "Failed to fetch user role";
      return false;
    }
  if (result)
    if (this->userRole =="")
      crash << "user role is empty" << crash;
//  stOutput << "Approximate hours since last token issued: " << this->approximateHoursSinceLastTokenWasIssued << ". ";
//  if (this->approximateHoursSinceLastTokenWasIssued>1 || this->approximateHoursSinceLastTokenWasIssued<=0)
  this->ResetAuthenticationToken(theRoutines, commentsOnFailure);
//  else if (result)
//    this->FetchOneColumn("authenticationToken", this->actualAuthenticationTokeNUnsafe, theRoutines, false, 0);
  return result;
}

bool UserCalculator::AuthenticateWithUserNameAndPass(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  if (!theGlobalVariables.flagIgnoreSecurityToWorkaroundSafarisBugs)
    this->ComputeShaonedSaltedPassword();
  else
    this->enteredShaonedSaltedPassword=this->enteredPassword;
//  stOutput <<  "computed shaoned saltes pass from pass "
//  << this->enteredPassword << " to get: " << this->enteredShaonedSaltedPassword;
  bool result=this->FetchOneColumn("password", this->actualShaonedSaltedPassword, theRoutines, commentsOnFailure);
  if (!result)
  { //stOutput << "fail remarks: " << failureRemarks;
    //stOutput << "pass doesnt match!";
    return false;
  }
//  *commentsOnFailure << "DEBUG INFO: this->enteredShaonedSaltedPassword: " << this->enteredShaonedSaltedPassword
//  << ", this->actualShaonedSaltedPassword: " << this->actualShaonedSaltedPassword;
  return this->enteredShaonedSaltedPassword==this->actualShaonedSaltedPassword;
}

bool UserCalculator::ResetAuthenticationToken(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::ResetAuthenticationToken");
  TimeWrapper now;
//  stOutput << "Resetting authentication token. ";
  now.AssignLocalTime();
  std::stringstream out;
  out << now.theTimeStringNonReadable << rand();
  this->actualAuthenticationToken=Crypto::computeSha1outputBase64(out.str());
  std::stringstream failure;
  if (!this->SetColumnEntry("authenticationToken", this->actualAuthenticationToken.value, theRoutines, &failure))
  { if (commentsOnFailure!=0)
      *commentsOnFailure << "Couldn't set column entry for token." << failure.str();
    return false;
  }
  this->flagNewAuthenticationTokenComputedUserNeedsIt=true;
  if (!this->SetColumnEntry("authenticationCreationTime", now.theTimeStringNonReadable, theRoutines, &failure))
  { if (commentsOnFailure!=0)
      *commentsOnFailure << "Couldn't set column entry for creation time. " << failure.str();
    return false;
  }
  //DatabaseQuery theQuery(*this, "SELECT authenticationCreationTime FROM calculatorUsers.users WHERE username=\""+this->username + "\"");
  //if (theQuery.flagQueryReturnedResult)
  //  this->authentication="authenticationCreationTime: "
  return true;
}

std::string UserCalculator::GetPassword(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::GetUserPassword");
  std::string storedShaonedSaltedPass;
  std::stringstream out;
  std::stringstream commentsOnFailure;
  if (!this->FetchOneColumn("password", storedShaonedSaltedPass, theRoutines, &commentsOnFailure))
    return commentsOnFailure.str();
  out << "password(sha-1): " << storedShaonedSaltedPass << "<br>comments: " << commentsOnFailure.str();
  return out.str();
}

bool UserCalculator::Iexist(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::Iexist");
  return this->IamPresentInTable(theRoutines, "users");
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

bool UserCalculator::DeleteMe(DatabaseRoutines& theRoutines, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::DeleteMe");
  if (MathRoutines::StringBeginsWith(this->username.value, "deleted"))
  { commentsOnFailure << "User names starting with 'deleted' cannot be deleted from the calculator interface. ";
    return false;
  }
  if (!this->Iexist(theRoutines))
  { commentsOnFailure << "I was not able to find user: " << this->username.value
    << ". The user does not exist or there is something wrong with the database";
    return false;
  }
  TimeWrapper currentTime;
  currentTime.AssignLocalTime();
  currentTime.ComputeTimeStringNonReadable();
  std::stringstream queryStream;
  MySQLdata deletedUserName="deleted"+currentTime.theTimeStringNonReadable+this->username.value;
  queryStream << "UPDATE users SET username=" << deletedUserName.GetDatA()
  << " WHERE username=" << this->username.GetDatA();
  DatabaseQuery renamingQuery(theRoutines, queryStream.str());
  return renamingQuery.flagQuerySucceeded;
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

void UserCalculator::ComputeShaonedSaltedPassword()
{ MacroRegisterFunctionWithName("UserCalculator::ComputeShaonedSaltedPassword");
  this->usernamePlusPassWord=this->username.value;
  this->usernamePlusPassWord+=this->enteredPassword;
  this->enteredShaonedSaltedPassword=Crypto::computeSha1outputBase64(this->usernamePlusPassWord);
  //<-careful copying those around. We want to avoid leaving
  //passwords in non-zeroed memory, even if properly freed (to the extent possible and practical).
  //Note the destructor of DatabaseRoutines zeroes some of the strings.
  //To do: make sure all crypto functions zero their buffers.
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

bool UserCalculator::SetColumnEntry
(const std::string& columnNameUnsafe, const std::string& theValueUnsafe, DatabaseRoutines& theRoutines,
 std::stringstream* failureComments)
{ MacroRegisterFunctionWithName("UserCalculator::SetColumnEntry");
  if (this->currentTable=="")
    crash << "Programming error: attempting to change column " << columnNameUnsafe << " without specifying a table. " << crash;
  return theRoutines.SetEntry((std::string) "username", this->username, this->currentTable, columnNameUnsafe, theValueUnsafe, failureComments);
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

bool UserCalculator::SetPassword(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::SetPassword");
  this->currentTable="users";
  if (this->enteredPassword=="")
  { if (commentsOnFailure!=0)
      *commentsOnFailure << "Empty password not allowed. ";
    return false;
  }
//  stOutput << "Whats going on here<br>";
  this->ComputeShaonedSaltedPassword();
//  std::stringstream mustdeleteme;
  return this->SetColumnEntry("password", this->enteredShaonedSaltedPassword, theRoutines, commentsOnFailure);
//  stOutput << mustdeleteme.str();
}

bool DatabaseRoutines::startMySQLDatabaseIfNotAlreadyStarted(std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::startMySQLDatabaseIfNotAlreadyStarted");
  if (this->connection!=0)
    return true;
  return this->startMySQLDatabase(commentsOnFailure, &this->flagFirstLogin);
}

bool DatabaseRoutines::startMySQLDatabase(std::stringstream* commentsOnFailure, bool* outputfirstLogin)
{ MacroRegisterFunctionWithName("DatabaseRoutines::startMySQLDatabase");
  if (theGlobalVariables.flagUsingBuiltInWebServer)
    if (!theGlobalVariables.flagUsingSSLinCurrentConnection &&
        !theGlobalVariables.flagIgnoreSecurityToWorkaroundSafarisBugs)
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

std::string DatabaseRoutines::GetTableUnsafeNameUsersOfFile(const std::string& inputFileName)
{ return "users"+ inputFileName;
}

bool DatabaseRoutines::CreateTable
(const std::string& tableNameUnsafe, const std::string& desiredTableContent, std::stringstream* commentsOnCreation, bool* outputTableNewlyCreated)
{ MacroRegisterFunctionWithName("DatabaseRoutines::CreateTable");
  if (tableNameUnsafe=="" )
  { if (commentsOnCreation!=0)
      *commentsOnCreation << "Empty table name not allowed.";
    return false;
  }
  if (this->TableExists(tableNameUnsafe, commentsOnCreation))
  { if (outputTableNewlyCreated!=0)
      *outputTableNewlyCreated=false;
    return true;
  }
  MySQLdata tableName=tableNameUnsafe;
  std::stringstream theQuery;
  theQuery << "CREATE TABLE " << tableName.GetIdentifieR();
  if (desiredTableContent=="")
    theQuery << "(username VARCHAR(255) NOT NULL PRIMARY KEY)";
  else
    theQuery << "(" << desiredTableContent << ")";
  //CANNOT use object DatabaseQuery as that object invokes startMySQLDatabase
  //which in turn invokes the present function.
//  if (commentsOnCreation!=0)
//    *commentsOnCreation << "Proceeding to create table: " << tableNameUnsafe << ". ";
  if (mysql_query(this->connection, theQuery.str().c_str())!=0)
  { if (commentsOnCreation!=0)
      *commentsOnCreation << "Command: " << theQuery.str() << " failed. Comments: "
      << mysql_error(this->connection);
    return false;
  }
  if (outputTableNewlyCreated!=0)
    *outputTableNewlyCreated=true;
  mysql_free_result( mysql_use_result(this->connection));
  return true;
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

bool DatabaseRoutines::TableExists(const std::string& tableNameUnsafe, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::TableExists");
  (void) commentsOnFailure;//portable way to avoid unused parameter warning
  if (this->connection==0)
    crash << "this->connection equals 0 at a place where this shouldn't happen. " << crash;
  MySQLdata tableName=tableNameUnsafe;
  std::stringstream queryStream;
  queryStream << "SELECT 1 FROM " << this->theDatabaseName << "." << tableName.GetIdentifieR();
  bool result=(mysql_query(this->connection, queryStream.str().c_str())==0);
//  if (!result)
//    if (commentsOnFailure!=0)
//      *commentsOnFailure << "Executed query to check table existence: " << queryStream.str() << ". ";
//  stOutput << "Executed query: " << queryStream.str() << "<br>";
  mysql_free_result( mysql_use_result(this->connection));
  return result;
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
  (void) comments;//portable way to avoid unused parameter warning
  List<unsigned char> delimiters;
  delimiters.AddOnTop(' ');
  delimiters.AddOnTop('\r');
  delimiters.AddOnTop('\n');
  delimiters.AddOnTop('\t');
  delimiters.AddOnTop(',');
  delimiters.AddOnTop(';');
  delimiters.AddOnTop(160);//<-&nbsp
  MathRoutines::StringSplitExcludeDelimiters(emailList, delimiters, outputList);
  return true;
}

bool DatabaseRoutines::SendActivationEmail(const std::string& emailList, std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::SendActivationEmail");
  List<std::string> theEmails;
  this->ExtractEmailList(emailList, theEmails, comments);
  return this->SendActivationEmail(theEmails, comments);
}

void UserCalculator::ComputePointsEarned
(const HashedList<std::string, MathRoutines::hashString>& gradableProblems,
 const List<std::string>& problemWeights)
{ MacroRegisterFunctionWithName("UserCalculator::ComputePointsEarned");
  this->pointsEarned=0;
  for (int i=0; i<this->problemData.size; i++)
  { ProblemData& currentP=this->problemData[i];
    this->problemData[i].Points=0;
    this->problemData[i].totalNumSubmissions=0;
    this->problemData[i].numCorrectlyAnswered=0;
    if (gradableProblems.Contains(this->problemNames[i]) )
    { this->problemData[i].ProblemWeightUserInput=
      problemWeights[gradableProblems.GetIndex(this->problemNames[i])];
      this->problemData[i].flagProblemWeightIsOK=
      this->problemData[i].ProblemWeight.AssignStringFailureAllowed
      (this->problemData[i].ProblemWeightUserInput);
    }
//    this->problemData[i].numAnswersSought=this->problemData[i].answerIds.size;
    for (int j=0; j<currentP.theAnswers.size; j++)
    { if (currentP.theAnswers[j].numCorrectSubmissions>0)
        currentP.numCorrectlyAnswered++;
      currentP.totalNumSubmissions+=currentP.theAnswers[j].numSubmissions;
    }
    if (this->problemData[i].flagProblemWeightIsOK && currentP.theAnswers.size>0)
    { currentP.Points=(currentP.ProblemWeight*currentP.numCorrectlyAnswered)/currentP.theAnswers.size;
      this->pointsEarned+= this->problemData[i].Points;
    }
  }
}

void UserCalculator::ComputeActivationToken()
{ MacroRegisterFunctionWithName("UserCalculator::ComputeActivationToken");
  TimeWrapper now;
  //stOutput << "Resetting authentication token. ";
  now.AssignLocalTime();
  std::stringstream activationTokenStream;
  activationTokenStream << now.theTimeStringNonReadable << rand();
  this->activationToken=Crypto::computeSha1outputBase64(activationTokenStream.str());
}

bool DatabaseRoutines::SendActivationEmail(const List<std::string>& theEmails, std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::SendActivationEmail");
  if (!this->ColumnExists("activationToken", "users", comments))
    if (!this->CreateColumn("activationToken", "users", comments))
    { comments << "Failed to create activationToken column. ";
      return false;
    }
/*  if (!this->ColumnExists("activationEmailSent", "users", comments))
    if (!this->CreateColumn("activationEmailSent", "users", comments))
    { comments << "Failed to create activationEmailSent column. ";
      return false;
    }*/
  UserCalculator currentUser;
  currentUser.currentTable="users";
  ProgressReport theReport;
  bool result=true;
  TimeWrapper now;
  now.AssignLocalTime();
  now.ComputeTimeStringNonReadable();
  std::string emailActivationLogFileName = "LogFileEmailsDebug"+now.theTimeStringNonReadable + ".html";
  logger emailActivationLogFile(theGlobalVariables.PhysicalPathOutputFolder+emailActivationLogFileName);
  emailActivationLogFile.MaxLogSize=10000000;
  for (int i=0; i<theEmails.size; i++)
  { std::stringstream reportStream;
    reportStream << "Sending activation email, user " << i+1 << " out of " << theEmails.size;
    emailActivationLogFile << "Sending activation email, user " << i+1 << " out of " << theEmails.size << " ... ";
    theReport.Report(reportStream.str());
    currentUser.username=theEmails[i];
    currentUser.email=theEmails[i];
    if (currentUser.activationToken=="" || currentUser.activationToken=="error")
    { comments << "<span style=\"color:red\">Failed to get an activation token for user: "
      << currentUser.username.value << "</span>";
      continue;
    }
    std::stringstream localComments;
    if (!currentUser.SendActivationEmail(*this, localComments))
    { comments << "<span style=\"color:red\"><b>Failed to send activation email to: </b></span> "
      << currentUser.username.value;
      comments << localComments.str();
      result=false;
      emailActivationLogFile << logger::red << localComments.str() << logger::endL;
      continue;
    }
    emailActivationLogFile << logger::green << " sent." << logger::endL;
  }
  return result;
}

ProblemData& UserCalculator::GetProblemDataAddIfNotPresent(const std::string& problemName)
{ MacroRegisterFunctionWithName("UserCalculator::GetProblemDataAddIfNotPresent");
  int theIndex=this->problemNames.GetIndex(problemName);
  if (theIndex==-1)
  { this->problemNames.AddOnTop(problemName);
    this->problemData.SetSize(this->problemData.size+1);
    return *this->problemData.LastObject();
  }
  return this->problemData[theIndex];
}

void UserCalculator::SetProblemData(const std::string& problemName, const ProblemData& inputData)
{ MacroRegisterFunctionWithName("UserCalculator::SetProblemData");
  int theIndex=this->problemNames.GetIndex(problemName);
  if (theIndex==-1)
  { this->problemNames.AddOnTop(problemName);
    this->problemData.AddOnTop(inputData);
  } else
    this->problemData[theIndex]=inputData;
}

bool ProblemData::LoadFrom(const std::string& inputData, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("ProblemData::LoadFrom");
  MapList<std::string, std::string, MathRoutines::hashString> theMap;
  if (!CGI::ChopCGIInputStringToMultipleStrings(inputData, theMap, commentsOnFailure))
    return false;
//  stOutput << "<hr>DEBUG: Interpreting: <br>" << inputData << "<hr>";
  this->flagRandomSeedComputed=false;
  if (theGlobalVariables.UserRequestRequiresLoadingRealExamData() )
    if (theMap.Contains("randomSeed"))
    { this->randomSeed=atoi(theMap.GetValueCreateIfNotPresent("randomSeed").c_str());
      this->flagRandomSeedComputed=true;
    }
  this->theAnswers.SetSize(0);
  bool result=true;
  MapList<std::string, std::string, MathRoutines::hashString> currentQuestionMap;
  for (int i=0; i<theMap.size(); i++)
  { if (theMap.theKeys[i]=="randomSeed")
      continue;
    this->AddEmptyAnswerIdOnTop(CGI::URLStringToNormal(theMap.theKeys[i]));
    Answer& currentA=*this->theAnswers.LastObject();
    std::string currentQuestion=CGI::URLStringToNormal(theMap.theValues[i]);
    result=CGI::ChopCGIInputStringToMultipleStrings
    (currentQuestion, currentQuestionMap, commentsOnFailure);
    if (!result)
    { commentsOnFailure << "Failed to interpret as key-value pair: "
      << currentQuestion << ". ";
      continue;
    }
    if (currentQuestionMap.Contains("numCorrectSubmissions"))
      currentA.numCorrectSubmissions=
      atoi(currentQuestionMap.GetValueCreateIfNotPresent("numCorrectSubmissions").c_str());
    if (currentQuestionMap.Contains("numSubmissions"))
      currentA.numSubmissions=
      atoi(currentQuestionMap.GetValueCreateIfNotPresent("numSubmissions").c_str());
    if (currentQuestionMap.Contains("firstCorrectAnswer"))
    { currentA.firstCorrectAnswerURLed=currentQuestionMap.GetValueCreateIfNotPresent("firstCorrectAnswer");
      currentA.firstCorrectAnswerClean= CGI::URLStringToNormal(currentA.firstCorrectAnswerURLed);
      currentA.firstCorrectAnswerURLed=CGI::StringToURLString(currentA.firstCorrectAnswerClean); //url-encoding back the cleaned up answer:
      //this protects from the possibility that currentA.firstCorrectAnswerURLed was not encoded properly.
    }
  }
  return result;
}

std::string ProblemData::Store()
{ MacroRegisterFunctionWithName("ProblemData::Store");
  std::stringstream out;
  out << "randomSeed=" << this->randomSeed;
  for (int i=0; i<this->theAnswers.size; i++)
  { Answer& currentA=this->theAnswers[i];
    out << "&" << CGI::StringToURLString(currentA.answerId) << "=";
    std::stringstream questionsStream;
    questionsStream
    << "numCorrectSubmissions=" << currentA.numCorrectSubmissions
    << "&numSubmissions=" << currentA.numSubmissions
    << "&firstCorrectAnswer=" << CGI::StringToURLString(currentA.firstCorrectAnswerClean);
    out << CGI::StringToURLString(questionsStream.str());
  }
  return out.str();
}

bool UserCalculator::InterpretDatabaseProblemData
(const std::string& theInfo, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::InterpretDatabaseProblemData");

  MapList<std::string, std::string, MathRoutines::hashString> theMap;
  if (!CGI::ChopCGIInputStringToMultipleStrings(theInfo, theMap, commentsOnFailure))
    return false;
  this->problemNames.Clear();
  this->problemData.SetSize(0);
  this->problemNames.SetExpectedSize(theMap.size());
  this->problemData.SetExpectedSize(theMap.size());
  bool result=true;
  for (int i=0; i<theMap.size(); i++)
  { this->problemNames.AddOnTop(CGI::URLStringToNormal(theMap.theKeys[i]));
    this->problemData.SetSize(this->problemData.size+1);
    if (!this->problemData.LastObject()->LoadFrom(CGI::URLStringToNormal(theMap[i]), commentsOnFailure))
      result=false;
  }
  return result;
}

bool UserCalculator::LoadProblemStringFromDatabase
(DatabaseRoutines& theRoutines, std::string& output, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::LoadProblemStringFromDatabase");
  this->currentTable="users";
  return this->FetchOneColumn("problemData", output, theRoutines, &commentsOnFailure);
}

bool UserCalculator::StoreProblemDataToDatabase
(DatabaseRoutines& theRoutines, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::StoreDatabaseInfo");
  std::stringstream problemDataStream;
  for (int i=0; i<this->problemNames.size; i++)
    problemDataStream << CGI::StringToURLString(this->problemNames[i]) << "="
    << CGI::StringToURLString( this->problemData[i].Store()) << "&";
  this->currentTable="users";
  return this->SetColumnEntry("problemData", problemDataStream.str(), theRoutines, &commentsOnFailure);
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

bool DatabaseRoutines::AddUsersFromEmails
  (bool doSendEmails, const std::string& emailList, const std::string& extraInfo, bool& outputSentAllEmails,
   std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::AddUsersFromEmails");
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=1000;
  theGlobalVariables.MaxComputationTimeBeforeWeTakeAction=1000;
  std::string userRole=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("userRole"));
  std::string extraUserInfo=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("extraInfo"));
  std::string currentExamHome=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
  std::string currentFileUsersTableName=this->GetTableUnsafeNameUsersOfFile(currentExamHome);
  List<std::string> theEmails;
  this->ExtractEmailList(emailList, theEmails, comments);
//  stOutput << " <br>creating users: " << theEmails.ToStringCommaDelimited() << "<br>";
  UserCalculator currentUser;
  bool result=true;
  if (!this->startMySQLDatabaseIfNotAlreadyStarted(&comments))
    return false;
  if (!this->TableExists(currentFileUsersTableName, &comments))
    if (!this->CreateTable(currentFileUsersTableName, "username VARCHAR(255) NOT NULL PRIMARY KEY, \
        extraInfo LONGTEXT ", &comments, 0))
      result=false;
  currentUser.currentTable="users";
  for (int i=0; i<theEmails.size; i++)
  { currentUser.username=theEmails[i];
    currentUser.email=theEmails[i];
    if (!currentUser.Iexist(*this))
    { if (!currentUser.CreateMeIfUsernameUnique(*this, &comments))
      { comments << "Failed to create user: " << currentUser.username.value;
        result=false;
        continue;
      }
      if (theEmails[i].find('@')!=std::string::npos)
        currentUser.SetColumnEntry("email", theEmails[i], *this, &comments);
    } else
      if (!currentUser.FetchOneUserRow(*this, comments))
        result=false;
    currentUser.SetColumnEntry("userRole", userRole, *this, &comments);
    currentUser.ComputeActivationToken();
    if (!currentUser.SetColumnEntry("activationToken", currentUser.activationToken.value, *this, &comments))
    { comments << "Setting activation token failed.";
      result=false;
      currentUser.activationToken="";
    }
  }
  currentUser.currentTable=currentFileUsersTableName;
  for (int i=0; i<theEmails.size; i++)
  { currentUser.username=theEmails[i];
    if (!currentUser.IamPresentInTable(*this, currentFileUsersTableName))
      if (!this->InsertRow("username", theEmails[i], currentFileUsersTableName, comments))
        result=false;
    if (!currentUser.SetColumnEntry("extraInfo", extraInfo, *this, &comments))
      result=false;
  }
  if (!result)
    comments << "<br>Failed to create all users. ";
  outputSentAllEmails=true;
  if (doSendEmails)
  { doSendEmails=false;
    outputSentAllEmails=false;
    comments << "Sending actual emails disabled for security reasons (system not stable enough yet). ";
  }
  if (doSendEmails)
    if (! this->SendActivationEmail(theEmails, comments))
      outputSentAllEmails=false;
  return result;
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

bool UserCalculator::GetActivationAbsoluteAddress
  (std::string& output, DatabaseRoutines& theRoutines,
   std::stringstream& comments)
{ MacroRegisterFunctionWithName("UserCalculator::GetActivationAbsoluteAddress");
  return this->GetActivationAddress
  (output, theGlobalVariables.hopefullyPermanent_HTTPS_WebAdressOfServerExecutable, theRoutines, comments);
}

bool UserCalculator::GetActivationAddress
  (std::string& output, const std::string& calculatorBase, DatabaseRoutines& theRoutines,
   std::stringstream& comments)
{ MacroRegisterFunctionWithName("UserCalculator::GetActivationAbsoluteAddress");
  if (!this->FetchOneUserRow(theRoutines, comments))
    return false;
  this->activationToken= this->GetSelectedRowEntry("activationToken");
  if (this->activationToken=="")
  { comments << "Failed to fetch activation token for user: " << this->username.value;
    return false;
  }
  if (this->activationToken=="activated")
  { comments << "Account of user: " << this->username.value << "already activated";
    return false;
  }
  output= this->GetActivationAddressFromActivationToken
  (this->activationToken.value, calculatorBase, this->username.value);
  return true;
}

bool UserCalculator::SendActivationEmail(DatabaseRoutines& theRoutines, std::stringstream& comments)
{ MacroRegisterFunctionWithName("UserCalculator::SendActivationEmail");
  std::string activationAddress;
  if (!this->GetActivationAbsoluteAddress(activationAddress, theRoutines, comments))
    return false;
//  stOutput << "<hr> all result strings: " << this->selectedRowFieldNamesUnsafe.ToStringCommaDelimited();
//  stOutput << "<br> all result string names: " << this->selectedRowFieldsUnsafe.ToStringCommaDelimited();
  this->email=this->GetSelectedRowEntry("email");
  //theGlobalVariables.FallAsleep(1000000);

  if (this->email=="")
  { comments << "\nNo email address for user: " << this->username.value << ". ";
    return false;
  }
  EmailRoutines theEmailRoutines;
  theEmailRoutines.toEmail=this->email.value;
  theEmailRoutines.subject="NO REPLY: Activation of a Math homework account. ";
  theEmailRoutines.emailContent="Activation link: " + activationAddress;
//  std::stringstream emailStream;
//  emailStream << "Dear student,\nthis is an automated email sent with an activation token for your "
//  << " math homework account. To activate your account and set up your password, please follow the link below. "
//  << " If you have any technical problems or have questions, please DO NOT HIT the REPLY BUTTON; "
//  << " instead, post your quesiton on piazza.com or email your question to "
//  << " todor.milev@gmail.com\n\n";
//  emailStream << this->activationTokenUnsafe << "\n\nGood luck with our course, \n Your calculus instructors.";
  std::string emailLog=theGlobalVariables.CallSystemWithOutput(theEmailRoutines.GetCommandToSendEmailWithMailX());
  size_t indexGoAhead= emailLog.find("Go ahead");
  bool result=true;
  if (indexGoAhead== std::string::npos)
    result=false;
  else
  { std::string choppedEmailLog=emailLog.substr(indexGoAhead+8);
    result=(choppedEmailLog.find("OK")!=std::string::npos);
  }
  //stOutput << "Calling system with:<br>" << theEmailRoutines.GetCommandToSendEmailWithMailX()
  //<< "<br>\n to get output: \n<br>" << emailLog;
  if (!result)
    comments << "\nFailed to send email to " << this->username.value << ". Details: \n" << emailLog;
  return result;
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
  (void) output;
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

bool DatabaseRoutines::innerDeleteUser(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerDeleteUser");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return output.MakeError("Deleting users allowed only as logged-in admin. ", theCommands);
  UserCalculator theUser;
  if (!theUser.getUser(theCommands, input))
    return false;
  DatabaseRoutines theRoutines;
  std::stringstream comments;
  theUser.DeleteMe(theRoutines, comments);
  return output.AssignValue(comments.str(), theCommands);
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
  if (theUser.username==theGlobalVariables.userDefault)
    authorized=true;
  else if (theGlobalVariables.UserDefaultHasAdminRights())
  { authorized=true;
    theCommands << "Password change: invoking admin powers...";
  }
  if (!authorized)
    return theCommands << "Password change failed for user " << theUser.username.value
    << ": not enough privileges. ";
  DatabaseRoutines theRoutines;
  std::stringstream comments;
  theUser.SetPassword(theRoutines, &comments);
  return output.AssignValue(comments.str(), theCommands);
}

bool UserCalculator::getUser(Calculator& theCommands, const Expression& input)
{ MacroRegisterFunctionWithName("UserCalculator::getUser");
  if (!input.IsOfType<std::string>(&this->username.value))
  { theCommands << "<hr>Argument " << input.ToString() << " is supposed to be a string.";
    this->username=input.ToString();
  }
  return true;
}

bool UserCalculator::getUserAndPass(Calculator& theCommands, const Expression& input)
{ MacroRegisterFunctionWithName("UserCalculator::getUserAndPass");
  if (input.children.size!=3)
    return theCommands << "UserCalculator::getUserAndPass takes as input 2 arguments (user name and password). ";
  if (!input[1].IsOfType<std::string>(&this->username.value))
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
  bool result = this->getUserPassAndExtraData(theCommands, input, this->selectedColumnsUnsafe);
  this->selectedColumnValuesUnsafe.initFillInObject(this->selectedColumnsUnsafe.size, "");
  this->selectedColumnsRetrievalFailureRemarks.initFillInObject(this->selectedColumnsUnsafe.size, "");
  return result;
}

bool UserCalculator::getUserPassAndExtraData(Calculator& theCommands, const Expression& input, List<std::string>& outputData)
{ MacroRegisterFunctionWithName("UserCalculator::getUserPassAndExtraData");
  if (input.children.size<4)
    return theCommands << "UserCalculator::getUserPassAndExtraData takes as input at least 2 arguments (user name and password). ";
  if (!input[1].IsOfType<std::string>(&this->username.value))
  { theCommands << "<hr>Argument " << input[1].ToString() << " is supposed to be a string.";
    this->username=input[1].ToString();
  }
  if (!input[2].IsOfType<std::string>(&this->enteredPassword))
  { theCommands << "<hr>Argument " << input[2].ToString() << " is supposed to be a string.";
    this->enteredPassword=input[2].ToString();
    this->enteredAuthenticationToken=this->enteredPassword;
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
  if (!input[1].IsOfType<std::string>(&this->username.value))
  { theCommands << "<hr>Argument " << input[1].ToString() << " is supposed to be a string.";
    this->username=input[1].ToString();
  }
  if (!input[2].IsOfType<std::string>(&this->enteredPassword))
  { theCommands << "<hr>Argument " << input[2].ToString() << " is supposed to be a string.";
    this->enteredPassword=input[2].ToString();
    this->enteredAuthenticationToken=this->enteredPassword;
  }
  if (!input[3].IsOfType<std::string>(&this->email.value))
  { theCommands << "<hr>Argument " << input[3].ToString() << " is supposed to be a string.";
    this->email=input[3].ToString();
  }
  return true;
}

void UserCalculator::FetchColumns(DatabaseRoutines& theRoutines)
{ MacroRegisterFunctionWithName("UserCalculator::FetchColumns");
  if (this->selectedColumnsUnsafe.size!=this->selectedColumnValuesUnsafe.size ||
      this->selectedColumnsUnsafe.size!=this->selectedColumnsRetrievalFailureRemarks.size)
    crash << "This is a programming error: selected columns not initialized properly" << crash;
  for (int i=0; i<this->selectedColumnsUnsafe.size; i++)
  { std::stringstream tempStream;
    this->FetchOneColumn
    (this->selectedColumnsUnsafe[i], this->selectedColumnValuesUnsafe[i], theRoutines, &tempStream);
    this->selectedColumnsRetrievalFailureRemarks[i]=tempStream.str();
  }
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

bool DatabaseRoutines::innerGetUserDetails(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerGetUserDetails");
  UserCalculator theUser;
  if (!theUser.getUserAndPass(theCommands, input))
    return false;
  DatabaseRoutines theRoutines;
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return output.AssignValue
    ((std::string)"At the moment, the GetUserDetails function is implemented only for admin user(s). ", theCommands);
  std::stringstream comments;
  if (!theUser.Authenticate(theRoutines, &comments))
    return theCommands << "Authentication failed. " << comments.str();
  return output.AssignValue(theRoutines.ToStringAllUsersHTMLFormat(), theCommands);
}

bool DatabaseRoutines::innerDisplayTables(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerDisplayTables");
  (void) input;//prevent unused parameter, portable
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

bool DatabaseRoutines::innerDisplayDatabaseTable
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerDisplayDatabaseTable");
  if (theGlobalVariables.UserDefaultHasAdminRights())
    return theCommands << "Displaying database tables allowed only for logged-in admins. ";
  std::string desiredTableName;
  if (!input.IsOfType<std::string>(&desiredTableName))
  { theCommands << "<hr>Argument " << input.ToString() << " is supposed to be a string. ";
    desiredTableName=input.ToString();
  }
  DatabaseRoutines theRoutines;
  return output.AssignValue(theRoutines.ToStringTablE(desiredTableName), theCommands);
}

bool DatabaseRoutines::innerGetUserDBEntry(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerGetauthenticationCreationTime");
  UserCalculator theUser;
  if (!theUser.getUserPassAndSelectedColumns(theCommands, input))
    return false;
  DatabaseRoutines theRoutines;
  std::stringstream comments;
  if (!theUser.Authenticate(theRoutines, &comments))
    return theCommands << comments.str();
  theUser.FetchColumns(theRoutines);
  return output.AssignValue(theUser.ToStringSelectedColumns(), theCommands);
}

bool DatabaseRoutines::innerGetAuthentication(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerGetAuthentication");
  UserCalculator theUser;
  if (!theUser.getUserAndPass(theCommands, input))
    return false;
  DatabaseRoutines theRoutines;
  std::stringstream comments;
  if (!theUser.Authenticate(theRoutines, &comments))
    return output.MakeError("Failed to authenticate. ", theCommands);
  return output.AssignValue(theUser.actualAuthenticationToken.value, theCommands);
}

bool DatabaseRoutines::innerTestDatabase(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerTestDatabase");
  (void) input;//prevent unused parameter, portable
  DatabaseRoutines theRoutines;
  std::stringstream out;
  out << "Testing database ... Comments:<br>";
  std::stringstream comments;
  theRoutines.startMySQLDatabase(&comments, 0);
  out << comments.str();
  return output.AssignValue(out.str(), theCommands);
}

std::string EmailRoutines::GetCommandToSendEmailWithMailX()
{ MacroRegisterFunctionWithName("EmailRoutines::GetCommandToSendEmailWithMailX");
  std::stringstream out;
  out << "echo "
  << "\""
  << this->emailContent
  << "\" "
  << "| mailx -v -s "
  << "\""
  << this->subject
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
  << "-S nss-config-dir=/etc/pki/nssdb/ "
  << this->toEmail;
  return out.str();
}

EmailRoutines::EmailRoutines()
{ this->fromEmail="calculator.todor.milev@gmail.com";
  //this->ccEmail="todor.milev@gmail.com";
  this->smtpWithPort= "smtp.gmail.com:587";
  this->fromEmailAuth=Crypto::CharsToBase64String("A good day to use a computer algebra system");
}
#endif // MACRO_use_MySQL

#ifdef MACRO_use_MySQL
std::string DatabaseRoutines::ToStringSuggestionsReasonsForFailure
(const std::string& inputUsernameUnsafe, DatabaseRoutines& theRoutines, UserCalculator& theUser)
{ MacroRegisterFunctionWithName("ToStringSuggestionsReasonsForFailure");
  std::stringstream out;
  bool userFound=false;
  if (theUser.Iexist(theRoutines))
    userFound=true;
  else if (theUser.username.value.find('@')==std::string::npos)
    { theUser.username.value+="@umb.edu";
      if (theUser.Iexist(theRoutines))
        userFound=true;
    }
  if (userFound)
  { std::string recommendedNewName;
    if (!theUser.FetchOneColumn("username", recommendedNewName, theRoutines, &out))
      return out.str();
    theUser.username.value=recommendedNewName;
  }
  if (theUser.username.value!=inputUsernameUnsafe)
  { out << "<br>Username is case sensitive and consists of the full email address: perhaps you meant: <br>"
    << theUser.username.value << "<br>";
    //out << "<hr>" << Crasher::GetStackTraceEtcErrorMessage() << "<hr>";
  }
  return out.str();
}
#endif // MACRO_use_MySQL

