//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader7DatabaseInterface_MySQL.h"
ProjectInformationInstance ProjectInfoVpfDatabaseSharedcpp(__FILE__, "MySQL interface, shared code. ");

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
      out << "<td>" << CGI::URLKeyValuePairsToNormalRecursiveHtml(theTable[i][j]) << "</td>";
    out << "</tr>";
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
  //if (!theGlobalVariables.flagIgnoreSecurityToWorkaroundSafarisBugs)
    this->ComputeShaonedSaltedPassword();
  //else
  //  this->enteredShaonedSaltedPassword=this->enteredPassword;
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
