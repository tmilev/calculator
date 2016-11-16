//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader7DatabaseInterface_MySQL.h"
#include "vpfHeader5Crypto.h"
#include "vpfHeader7DatabaseInterface_MySQL.h"

ProjectInformationInstance ProjectInfoVpf8_1MySQLcpp(__FILE__, "MySQL interface. ");

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
  return DatabaseRoutinesGlobalFunctions::FetchEntry
  (theGlobalVariables.userDefault.username.value, "instructors", DatabaseStrings::usersTableName, notUsed, comments);
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

bool DatabaseRoutinesGlobalFunctions::LogoutViaDatabase()
{
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::LogoutViaDatabase");
  //stOutput << "Got to logout";
  if (!theGlobalVariables.flagLoggedIn)
  { //stOutput << "but not resetting token. ";
    return true;
  }
  DatabaseRoutines theRoutines;
  UserCalculator theUser;
  theUser.UserCalculatorData::operator=(theGlobalVariables.userDefault);
  theUser.currentTable=DatabaseStrings::usersTableName;
//  stOutput << "<hr>DEBUG: logout: resetting token ... ";
  theUser.ResetAuthenticationToken(theRoutines, 0);
//  stOutput << "token reset!... <hr>";
  theGlobalVariables.SetWebInpuT("authenticationToken", "");
  return true;
#else
  return true;
#endif
}

std::string DatabaseStrings::userId="id";
std::string DatabaseStrings::userColumnLabel="username";
std::string DatabaseStrings::usersTableName="users";
std::string DatabaseStrings::userGroupLabel="userInfo";
std::string DatabaseStrings::databaseUser="ace";
std::string DatabaseStrings::theDatabaseName="aceDB";
std::string DatabaseStrings::deadlinesTableName="deadlines";
std::string DatabaseStrings::deadlinesIdColumnName="idInDeadlines";
std::string DatabaseStrings::infoColumnInDeadlinesTable="deadlines";

std::string DatabaseStrings::problemWeightsTableName="problemWeights";
std::string DatabaseStrings::problemWeightsIdColumnName="idInProblemInfo";
std::string DatabaseStrings::infoColumnInProblemWeightsTable="problemWeights";

std::string DatabaseStrings::sectionsTaughtByUser="sectionList";

std::string MySQLdata::GetDatA()const
{ return "'" + this->GetDataNoQuotes() + "'";
}

std::string MySQLdata::GetDataNoQuotes()const
{ return CGI::StringToURLString(this->value);
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

ProblemData::ProblemData()
{ this->randomSeed=0;
  this->flagRandomSeedGiven=false;
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

bool ProblemDataAdministrative::GetWeightFromSection
(const std::string& theSection, Rational& output, std::string *outputAsGivenByInstructor)
{ MacroRegisterFunctionWithName("ProblemDataAdministrative::GetWeightFromSection");
  if (!this->problemWeightsPerSectionDB.Contains(theSection))
    return false;
  std::string tempString;
  if (outputAsGivenByInstructor==0)
    outputAsGivenByInstructor=&tempString;
  *outputAsGivenByInstructor=this->problemWeightsPerSectionDB.GetValueCreateIfNotPresent(theSection);
  return output.AssignStringFailureAllowed(*outputAsGivenByInstructor);
}

std::string ProblemDataAdministrative::ToString()const
{ std::stringstream out;
  out << this->deadlinesPerSection.ToStringHtml();
  out << this->problemWeightsPerSectionDB.ToStringHtml();
  return out.str();
}

std::string ProblemData::ToString()const
{ std::stringstream out;
  out << "Problem data. "
  << "Random seed: " << this->randomSeed;
  if (this->flagRandomSeedGiven)
    out << " (given)";
  out << ". ";
  for (int i=0; i<this->theAnswers.size; i++)
  { Answer& currentA=this->theAnswers[i];
    out << "AnswerId: " << currentA.answerId;
    out << ", numCorrectSubmissions: " << currentA.numCorrectSubmissions;
    out << ", numSubmissions: " << currentA.numSubmissions;
    out << ", firstCorrectAnswer: ";
    if (currentA.firstCorrectAnswerClean=="")
      out << "[none yet], ";
    else
      out << "[" << currentA.firstCorrectAnswerClean << "], ";
  }
  out << "Admin data: " << this->adminData.ToString();
  return out.str();
}

#ifdef MACRO_use_MySQL
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
  int indexToBeReplacedWithLinks=columnLabels.GetIndex("problemData");
//  out << "DEBUG: <b>HERE AM I</b> Column labels: " << columnLabels.ToStringCommaDelimited()
//  << ". indexToBeReplacedWithLinks: " << indexToBeReplacedWithLinks;
  out << "<style>span.linkLike"
  << "{\n"
  << "color: blue;\n "
  << "text-decoration:\n "
  << "underline;\n "
  << "cursor: pointer;\n"
  << "}" << "</style>";
  out << "<table><tr>";
  for (int i=0; i<columnLabels.size; i++)
    out << "<td>" << columnLabels[i] << "</td>";
  out << "</tr>";
  for (int i=0; i<theTable.size; i++)
  { out << "<tr>";
    for (int j=0; j<theTable[i].size; j++)
      if (j!=indexToBeReplacedWithLinks)
        out << "<td>" << CGI::URLKeyValuePairsToNormalRecursiveHtml(theTable[i][j]) << "</td>";
      else if (theTable[i][j]=="")
        out << "<td></td>";
      else
      { out << "<td>";
        std::stringstream spanId;
        std::stringstream theRequest;
        theRequest
        << "currentDatabaseTable=" << CGI::StringToURLString(tableIdentifier) << "&"
        << "currentDatabaseRow=" << CGI::StringToURLString(theTable[i][0]) << "&"
        << "currentDatabaseKeyColumn=" << CGI::StringToURLString(columnLabels[0]) << "&"
        << "currentDatabaseDesiredColumn=" << CGI::StringToURLString(columnLabels[indexToBeReplacedWithLinks])
        ;
        spanId << "spanRow" << i << "Col" << j;
        out << "<span class=\"linkLike\"  "
        << "onclick=\"submitStringAsMainInput("
        << "'" << theRequest.str() << "', "
        << "'" << spanId.str() << "', "
        << "'databaseOneEntry'"
        << "); return true;\">More info</span> <span id=\""
        << spanId.str() << "\"></span>";
        out << "</td>";
      }
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

std::string DatabaseRoutines::ToStringCurrentTableHTML ()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringCurrentTableHTML");
  std::string currentTable=
  CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentDatabaseTable"), false);
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

std::string DatabaseRoutines::ToStringOneEntry()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringOneEntry");
  MapLisT<std::string, std::string, MathRoutines::hashString> theMap;
  std::stringstream out;
  if (!CGI::ChopCGIString(CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"), false), theMap, out))
  { out << "Failed to extract input arguments. ";
    return out.str();
  }
  out << "<hr>";
//  out << "DEBUG: getting info from: " << theGlobalVariables.GetWebInput("mainInput")
//  << "<br> got: <hr>" << theMap.ToStringHtml();
  std::string currentTable=CGI::URLStringToNormal(theMap.GetValueCreateIfNotPresent
  ("currentDatabaseTable"), false);
  std::string currentRow = CGI::URLStringToNormal(theMap.GetValueCreateIfNotPresent
  ("currentDatabaseRow"), false);
  std::string currentKeyColumn = CGI::URLStringToNormal(theMap.GetValueCreateIfNotPresent
  ("currentDatabaseKeyColumn"), false);
  std::string currentDesiredColumn = CGI::URLStringToNormal(theMap.GetValueCreateIfNotPresent
  ("currentDatabaseDesiredColumn"), false);

  if (currentTable.find("`")!=std::string::npos ||
      currentRow.find("`")!=std::string::npos ||
      currentKeyColumn.find("`")!=std::string::npos ||
      currentDesiredColumn.find("`")!=std::string::npos
      )
  { std::stringstream out;
    out << "<b>At least one of the identifiers: "
    << currentTable << ", " << currentRow << ", " << currentKeyColumn << ", " << currentDesiredColumn
    << " contains the ` character and is invalid. </b>";
    return out.str();
  }
  if (currentTable.size()>=64 || currentRow.size()>=64 ||
      currentKeyColumn.size()>=64 || currentDesiredColumn.size()>=64)
  { std::stringstream out;
    out << "<b>At least one of the identifiers: "
    << currentTable << ", " << currentRow << ", " << currentKeyColumn << ", " << currentDesiredColumn
    << " is longer than 64 characters and is invalid. </b>";
    return out.str();
  }
  std::string outputString;
  if (!this->FetchEntry
      (currentKeyColumn, currentRow, currentTable, currentDesiredColumn, outputString, &out))
    out << "<hr>Failed to fetch entry. ";
  else
    out
    //<< "<hr>DEBUG<hr>"
    << CGI::URLKeyValuePairsToNormalRecursiveHtml( outputString)
//    << "<hr>DEBUG<hr>"
    ;
//  out << "<hr>DEBUG: requesting: "
//  << currentTable << ", " << currentRow << ", " << currentKeyColumn << ", " << currentDesiredColumn
//  ;
  return out.str();
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
  out << "Calculator user: " << this->username.value << "<br>Section: " << this->userGroup.value
  << "<br>Sections taught: " << this->sectionsTaughtByUserString.value
  << ""
  ;

  Rational weightRat;
  for (int i=0; i<this->theProblemData.size(); i++)
  { out << "<br>Problem: " << this->theProblemData.theKeys[i] << "; random seed: "
    << this->theProblemData.theValues[i].randomSeed << "; numSubmissions: "
    << this->theProblemData.theValues[i].totalNumSubmissions
    << "; correct: "
    << this->theProblemData.theValues[i].numCorrectlyAnswered
    << "; points: "
    << this->theProblemData.theValues[i].Points
    << ";";
    if (!this->theProblemData.theValues[i].adminData.GetWeightFromSection(this->userGroup.value, weightRat))
      out << " (weight not available). ";
    else
      out << " weight: " << weightRat.ToString();
  }
  out << "<br>Deadline info: " << CGI::URLKeyValuePairsToNormalRecursiveHtml(this->deadlineInfoString.value);
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

bool UserCalculator::FetchOneColumn
(const std::string& columnNameUnsafe, std::string& outputUnsafe,
 DatabaseRoutines& theRoutines, std::stringstream* failureComments)
{ MacroRegisterFunctionWithName("UserCalculator::FetchOneColumn");
  return theRoutines.FetchEntry
  (DatabaseStrings::userColumnLabel, this->username, this->currentTable,
   columnNameUnsafe, outputUnsafe, failureComments);
}

bool UserCalculator::AuthenticateWithToken(std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::AuthenticateWithToken");
  (void) commentsOnFailure;
//  stOutput << "<br>DEBUG: token-authenticating user: " << this->username.value << " with entered token: " << this->enteredAuthenticationToken.value
//  << " against actual token: " << this->actualAuthenticationToken.value;
  if (this->enteredAuthenticationToken=="")
    return false;
  TimeWrapper now;
  now.AssignLocalTime();
  this->approximateHoursSinceLastTokenWasIssued=now.SubtractAnotherTimeFromMeAndGet_APPROXIMATE_ResultInHours
  (this->authenticationCreationTime);
  //<-to do: revoke authentication token if expired.
  return this->enteredAuthenticationToken.value==this->actualAuthenticationToken.value;
}

bool UserCalculator::FetchOneUserRow
(DatabaseRoutines& theRoutines, std::stringstream* failureStream)
{ MacroRegisterFunctionWithName("UserCalculator::FetchOneUserRow");
  if (this->currentTable=="")
    crash << "Calling UserCalculator::FetchOneUserRow with an empty table is forbidden. " << crash;
  std::stringstream queryStream;
  queryStream << "SELECT * FROM " << theRoutines.theDatabaseName << "." << this->currentTable.GetIdentifieR()
  << " WHERE " << "username=" << this->username.GetDatA();
  DatabaseQuery theQuery(theRoutines, queryStream.str(), failureStream, 5);
  if (!theQuery.flagQuerySucceeded)
  { if (failureStream!=0)
      *failureStream << "Query: " << queryStream.str() << " failed. ";
    return false;
  }
  if (!theQuery.flagQueryReturnedResult)
  { if (failureStream!=0)
      *failureStream << "The table appears to be empty: query: " << queryStream.str()
      << " succeeded but returned no result. ";
    return false;
  }
  if (theQuery.allQueryResultStrings.size!=1)
  { if (failureStream!=0)
      *failureStream << "Could not find entries for user " << this->username.value << ".";
    return false;
  }
  this->selectedRowFieldsUnsafe.SetSize(theQuery.allQueryResultStrings[0].size);
  for (int i=0; i<this->selectedRowFieldsUnsafe.size; i++)
    this->selectedRowFieldsUnsafe[i]=CGI::URLStringToNormal(theQuery.allQueryResultStrings[0][i], false);
  theQuery.close();
  std::stringstream queryStreamFields;
  queryStreamFields
  << "SELECT `COLUMN_NAME` FROM information_schema.COLUMNS WHERE "
  << "TABLE_SCHEMA='" << theRoutines.theDatabaseName << "' "
  << "AND TABLE_NAME='" << this->currentTable.GetIdentifierNoQuotes() << "' ";
  DatabaseQuery theFieldQuery(theRoutines, queryStreamFields.str(), failureStream);
  if (!theFieldQuery.flagQuerySucceeded)
  { if (failureStream!=0)
      *failureStream << "Query: " << queryStreamFields.str() << " failed. ";
    return false;
  }
  this->selectedRowFieldNamesUnsafe.SetSize(theFieldQuery.allQueryResultStrings.size);
  for (int i=0; i<theFieldQuery.allQueryResultStrings.size; i++)
    if (theFieldQuery.allQueryResultStrings[i].size>0 )
      this->selectedRowFieldNamesUnsafe[i]=CGI::URLStringToNormal(theFieldQuery.allQueryResultStrings[i][0], false);
  if (this->currentTable!=DatabaseStrings::usersTableName)
    return true;
  this->actualActivationToken= this->GetSelectedRowEntry("activationToken");
  this->userId=this->GetSelectedRowEntry(DatabaseStrings::userId);
  this->email= this->GetSelectedRowEntry("email");
  this->userRole= this->GetSelectedRowEntry("userRole");
  this->username=this->GetSelectedRowEntry(DatabaseStrings::userColumnLabel);
  //<-Important! Database lookup may be
  //case insensitive (this shouldn't be the case, so welcome to the insane design of mysql).
  //The preceding line of code guarantees we have read the username as it is stored in the DB.
  if (theGlobalVariables.UserStudentViewOn() && this->userRole=="admin" &&
      theGlobalVariables.GetWebInput("studentSection")!="")
  //<- warning, the user may not be
  //fully logged-in yet so theGlobalVariables.UserDefaultHasAdminRights()
  //does not work right.
    this->userGroup=theGlobalVariables.GetWebInput("studentSection");
  else
    this->userGroup=this->GetSelectedRowEntry(DatabaseStrings::userGroupLabel);

  this->actualShaonedSaltedPassword=this->GetSelectedRowEntry("password");
  this->authenticationTokenCreationTime=this->GetSelectedRowEntry("authenticationCreationTime");
  this->actualAuthenticationToken=this->GetSelectedRowEntry("authenticationToken");
  this->problemDataString=this->GetSelectedRowEntry("problemData");
  this->deadlineInfoRowId=this->GetSelectedRowEntry(DatabaseStrings::deadlinesIdColumnName);
  this->problemInfoRowId=this->GetSelectedRowEntry(DatabaseStrings::problemWeightsIdColumnName);
  this->sectionsTaughtByUserString=this->GetSelectedRowEntry(DatabaseStrings::sectionsTaughtByUser);
  std::string reader;
  //stOutput << "DEBUG:  GOT to hereE!!!";
  if (this->deadlineInfoRowId!="")
    if (theRoutines.FetchEntry
        (DatabaseStrings::deadlinesIdColumnName,
         this->deadlineInfoRowId,
         DatabaseStrings::deadlinesTableName,
         DatabaseStrings::infoColumnInDeadlinesTable,
         reader,
         failureStream))
      this->deadlineInfoString=reader;
//  stOutput << "DEBUG: deadlineInfo, rawest: " << reader
//  << " this->deadlineInfoString:  " << this->deadlineInfoString.value;
  if (this->problemInfoRowId!="")
    if (theRoutines.FetchEntry
        (DatabaseStrings::problemWeightsIdColumnName,
         this->problemInfoRowId,
         DatabaseStrings::problemWeightsTableName,
         DatabaseStrings::infoColumnInProblemWeightsTable,
         reader,
         failureStream))
      this->problemInfoString=reader;
  return true;
}

bool UserCalculator::Authenticate(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  //stOutput << "<hr>DEBUG: Authenticating user: " << this->username.value << " with password: " << this->enteredPassword;
  this->currentTable=DatabaseStrings::usersTableName;
  std::stringstream secondCommentsStream;
  if (!this->FetchOneUserRow(theRoutines, &secondCommentsStream))
  { if (!this->Iexist(theRoutines))
      if (commentsOnFailure!=0)
        *commentsOnFailure << "User " << this->username.value << " does not exist. ";
  //stOutput << "<hr>DEBUG: user " << this->username.value << " does not exist. More details: " << secondCommentsStream.str() << "<hr>";
    return false;
  }
  if (this->AuthenticateWithToken(&secondCommentsStream))
    return true;
  //stOutput << "<br>DEBUG: User could not authenticate with token.";
  bool result= this->AuthenticateWithUserNameAndPass(theRoutines, commentsOnFailure);
  if (this->enteredPassword!="")
    this->ResetAuthenticationToken(theRoutines, commentsOnFailure);
  //<- this needs to be fixed: an attacker may cause denial of service by launching fake login attempts.
  return result;
}

bool UserCalculator::AuthenticateWithUserNameAndPass(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  (void) theRoutines;
  (void) commentsOnFailure;
  this->ComputeShaonedSaltedPassword();
  //stOutput << "DEBUG: computed salted shaoned pass: " << this->enteredShaonedSaltedPassword
  //<< "; actual: " << this->actualShaonedSaltedPassword;
  return this->enteredShaonedSaltedPassword==this->actualShaonedSaltedPassword;
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
  return this->IamPresentInTable(theRoutines, DatabaseStrings::usersTableName);
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

bool UserCalculator::SetColumnEntry
(const std::string& columnNameUnsafe, const std::string& theValueUnsafe, DatabaseRoutines& theRoutines,
 std::stringstream* failureComments)
{ MacroRegisterFunctionWithName("UserCalculator::SetColumnEntry");
  if (this->currentTable=="")
    crash << "Programming error: attempting to change column " << columnNameUnsafe << " without specifying a table. " << crash;
  return theRoutines.SetEntry(DatabaseStrings::userColumnLabel, this->username, this->currentTable, columnNameUnsafe, theValueUnsafe, failureComments);
}

bool UserCalculator::ResetAuthenticationToken(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::ResetAuthenticationToken");
  TimeWrapper now;
//  stOutput << "<hr>Resetting authentication token... ";
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
  //stOutput << "done. Token reset to: " << this->actualAuthenticationToken.value << "<br>Call stack: " << crash.GetStackTraceEtcErrorMessage()
  //<< "<hr>";
  //if (theQuery.flagQueryReturnedResult)
  //  this->authentication="authenticationCreationTime: "
  return true;
}

bool UserCalculator::SetPassword(DatabaseRoutines& theRoutines, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::SetPassword");
  this->currentTable=DatabaseStrings::usersTableName;
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

std::string DatabaseRoutines::GetTableUnsafeNameUsersOfFile(const std::string& inputFileName)
{ return DatabaseStrings::usersTableName+ inputFileName;
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

bool DatabaseRoutines::FetchAllUsers
(List<List<std::string> >& outputUserTable, List<std::string>& outputLabelsUserTable,
 std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::FetchAllUsers");
  DatabaseRoutines theRoutines;
  if (!theRoutines.startMySQLDatabaseIfNotAlreadyStarted(&commentsOnFailure))
  { commentsOnFailure << "<b>Failed to start database</b>";
    return false;
  }
  bool tableTruncated=false;
  int numRows=-1;
  if (!DatabaseRoutinesGlobalFunctions::FetchTablE
      (outputUserTable, outputLabelsUserTable, tableTruncated, numRows, DatabaseStrings::usersTableName, commentsOnFailure))
  { commentsOnFailure << "<span style=\"color:red\"><b>Failed to fetch table: "
    << DatabaseStrings::usersTableName << "</b></span>";
    return false;
  }
  if (tableTruncated)
  { commentsOnFailure << "<span style=\"color:red\"><b>This shouldn't happen: email list truncated. "
    << "This is likely a software bug. Function: DatabaseRoutines::FetchAllUsers. </b></span>";
    return false;
  }
  return true;
}

bool DatabaseRoutines::ExtractEmailList(const std::string& emailList, List<std::string>& outputList, std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::ExtractEmailList");
  (void) comments;//portable way to avoid unused parameter warning
  MathRoutines::StringSplitDefaultDelimiters(emailList, outputList);
  return true;
}

bool DatabaseRoutines::SendActivationEmail(const std::string& emailList, std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::SendActivationEmail");
  List<std::string> theEmails;
  this->ExtractEmailList(emailList, theEmails, comments);
  return this->SendActivationEmail(theEmails, comments);
}

void UserCalculator::ComputeActivationToken()
{ MacroRegisterFunctionWithName("UserCalculator::ComputeActivationToken");
  TimeWrapper now;
  //stOutput << "Resetting authentication token. ";
  now.AssignLocalTime();
  std::stringstream activationTokenStream;
  activationTokenStream << now.theTimeStringNonReadable << rand();
  this->actualActivationToken=Crypto::computeSha1outputBase64(activationTokenStream.str());
}

bool DatabaseRoutines::SendActivationEmail(const List<std::string>& theEmails, std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::SendActivationEmail");
  if (!this->ColumnExists("activationToken", DatabaseStrings::usersTableName, comments))
    if (!this->CreateColumn("activationToken", DatabaseStrings::usersTableName, comments))
    { comments << "Failed to create activationToken column. ";
      return false;
    }
/*  if (!this->ColumnExists("activationEmailSent", "users", comments))
    if (!this->CreateColumn("activationEmailSent", "users", comments))
    { comments << "Failed to create activationEmailSent column. ";
      return false;
    }*/
  UserCalculator currentUser;
  currentUser.currentTable=DatabaseStrings::usersTableName;
  ProgressReport theReport;
  bool result=true;
  TimeWrapper now;
  now.AssignLocalTime();
  now.ComputeTimeStringNonReadable();
  std::string emailActivationLogFileName = "LogFileEmailsDebug"+now.theTimeStringNonReadable + ".html";
  logger emailActivationLogFile(theGlobalVariables.PhysicalPathHtmlFolder+emailActivationLogFileName);
  emailActivationLogFile.MaxLogSize=10000000;
  for (int i=0; i<theEmails.size; i++)
  { std::stringstream reportStream;
    reportStream << "Sending activation email, user " << i+1 << " out of " << theEmails.size;
    emailActivationLogFile << "Sending activation email, user " << i+1 << " out of " << theEmails.size << " ... ";
    theReport.Report(reportStream.str());
    currentUser.username=theEmails[i];
    currentUser.email=theEmails[i];
    if (currentUser.actualActivationToken=="" || currentUser.actualActivationToken=="error")
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
  return this->theProblemData.GetValueCreateIfNotPresent(problemName);
}

void UserCalculator::SetProblemData(const std::string& problemName, const ProblemData& inputData)
{ MacroRegisterFunctionWithName("UserCalculator::SetProblemData");
  this->theProblemData.SetKeyValue(problemName, inputData);
}

bool ProblemData::LoadFrom(const std::string& inputData, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("ProblemData::LoadFrom");
  MapLisT<std::string, std::string, MathRoutines::hashString> theMap;
  if (!CGI::ChopCGIString(inputData, theMap, commentsOnFailure))
    return false;
  //stOutput << "<hr>DEBUG: Interpreting: <br>" << CGI::URLKeyValuePairsToNormalRecursiveHtml( inputData )<< "<hr>";
  this->Points=0;
  this->numCorrectlyAnswered=0;
  this->totalNumSubmissions=0;
  this->flagRandomSeedGiven=false;
  if (theGlobalVariables.UserRequestRequiresLoadingRealExamData())
  { if (theMap.Contains("randomSeed"))
    { this->randomSeed=atoi(theMap.GetValueCreateIfNotPresent("randomSeed").c_str());
      this->flagRandomSeedGiven=true;
      //stOutput << "<br>DEBUG: random seed found. <br>";
    } //else
      //stOutput << "<br>DEBUG: random seed  NOT NOT NOT found. <br>";
  }
  this->theAnswers.SetSize(0);
  bool result=true;
  MapLisT<std::string, std::string, MathRoutines::hashString> currentQuestionMap;
  for (int i=0; i<theMap.size(); i++)
  { if (theMap.theKeys[i]=="randomSeed")
      continue;
    this->AddEmptyAnswerIdOnTop(CGI::URLStringToNormal(theMap.theKeys[i], false));
    Answer& currentA=*this->theAnswers.LastObject();
    std::string currentQuestion=CGI::URLStringToNormal(theMap.theValues[i], false);
    result=CGI::ChopCGIString(currentQuestion, currentQuestionMap, commentsOnFailure);
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
      currentA.firstCorrectAnswerClean= CGI::URLStringToNormal(currentA.firstCorrectAnswerURLed, false);
      currentA.firstCorrectAnswerURLed=CGI::StringToURLString(currentA.firstCorrectAnswerClean); //url-encoding back the cleaned up answer:
      //this protects from the possibility that currentA.firstCorrectAnswerURLed was not encoded properly.
    }
  }
//  this->CheckConsistency();
  return result;
}

bool ProblemData::CheckConsistency()const
{ MacroRegisterFunctionWithName("ProblemData::CheckConsistency");
  for (int i=0; i<this->theAnswers.size; i++)
  { if (MathRoutines::StringTrimWhiteSpace(this->theAnswers[i].answerId)=="")
      crash << "This is not supposed to happen: empty answer id." << crash;
//    if (MathRoutines::StringTrimWhiteSpace(this->theAnswers[i].idMQfield)=="")
//      crash << "This is not supposed to happen: empty idMQfield. The answer id is: "
//      << this->theAnswers[i].answerId << "<br>" << this->ToString() << "<hr>All the answers are: "
//      << this->ToString() << crash;
  }
  return true;
}

bool ProblemData::CheckConsistencyMQids()const
{ MacroRegisterFunctionWithName("ProblemData::CheckConsistencyMQids");
  for (int i=0; i<this->theAnswers.size; i++)
  { if (MathRoutines::StringTrimWhiteSpace(this->theAnswers[i].idMQfield)=="")
      crash << "This is not supposed to happen: empty idMQfield. The answer id is: "
      << this->theAnswers[i].answerId << "<br>" << this->ToString() << "<hr>All the answers are: "
      << this->ToString() << crash;
  }
  return true;
}

std::string ProblemData::Store()
{ MacroRegisterFunctionWithName("ProblemData::Store");
  std::stringstream out;
  if (this->flagRandomSeedGiven)
    out << "randomSeed=" << this->randomSeed;
  for (int i=0; i<this->theAnswers.size; i++)
  { Answer& currentA=this->theAnswers[i];
    if (this->flagRandomSeedGiven || i!=0)
      out << "&";
    out << CGI::StringToURLString(currentA.answerId) << "=";
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
  MapLisT<std::string, std::string, MathRoutines::hashString> theMap;
  if (!CGI::ChopCGIString(theInfo, theMap, commentsOnFailure))
    return false;
  this->theProblemData.Clear();
  this->theProblemData.SetExpectedSize(theMap.size());
  bool result=true;
  //stOutput << "<hr>DEBUG: Interpreting: <br>" << CGI::URLKeyValuePairsToNormalRecursiveHtml(theInfo)
  //<< "<br>Map has: "
  //<< theMap.size() << " entries. ";
  ProblemData reader;
  std::string probNameNoWhiteSpace;
  for (int i=0; i<theMap.size(); i++)
  { //stOutput << "<hr>Reading data: " << theMap.theKeys[i] << ", value: "
    //<< CGI::URLKeyValuePairsToNormalRecursiveHtml(theMap[i]);
    if (!reader.LoadFrom(CGI::URLStringToNormal(theMap[i], false), commentsOnFailure))
    { result=false;
      continue;
    }
    probNameNoWhiteSpace=MathRoutines::StringTrimWhiteSpace(CGI::URLStringToNormal(theMap.theKeys[i], false));
    if (probNameNoWhiteSpace=="")
      continue;
    this->theProblemData.SetKeyValue(probNameNoWhiteSpace, reader);
  }
  return result;
}

bool UserCalculator::LoadProblemStringFromDatabase
(DatabaseRoutines& theRoutines, std::string& output, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::LoadProblemStringFromDatabase");
  this->currentTable=DatabaseStrings::usersTableName;
  return this->FetchOneColumn("problemData", output, theRoutines, &commentsOnFailure);
}

bool UserCalculator::StoreProblemDataToDatabase
(DatabaseRoutines& theRoutines, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::StoreProblemDataToDatabase");
  std::stringstream problemDataStream;
  for (int i=0; i<this->theProblemData.size(); i++)
    problemDataStream << CGI::StringToURLString(this->theProblemData.theKeys[i]) << "="
    << CGI::StringToURLString(this->theProblemData.theValues[i].Store()) << "&";
//  stOutput << "DEBUG: storing in database ... stack trace: "
//  << crash.GetStackTraceEtcErrorMessage();
  //<< CGI::URLKeyValuePairsToNormalRecursiveHtml(problemDataStream.str());
  this->currentTable=DatabaseStrings::usersTableName;
  bool result= this->SetColumnEntry("problemData", problemDataStream.str(), theRoutines, &commentsOnFailure);
  return result;
}

bool DatabaseRoutines::AddUsersFromEmails
(const std::string& emailList, const std::string& userPasswords, std::string& userRole, std::string& userGroup,
 std::stringstream& comments)
{ MacroRegisterFunctionWithName("DatabaseRoutines::AddUsersFromEmails");
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=1000;
  theGlobalVariables.MaxComputationTimeBeforeWeTakeAction=1000;
  List<std::string> theEmails, thePasswords;
  this->ExtractEmailList(emailList, theEmails, comments);
  this->ExtractEmailList(userPasswords, thePasswords, comments);
  if (thePasswords.size>0)
    if (thePasswords.size!=theEmails.size)
    { comments << "Different number of usernames/emails and passwords: " << theEmails.size << " emails and "
      << thePasswords.size << " passwords. "
      << "If you want to enter usernames without password, you need to leave the password input box empty. ";
      return false;
    }
  //stOutput << " <br>Debug: creating users: " << theEmails.ToStringCommaDelimited() << "<br>Number of users: " << theEmails.size
  //<< "<br>Number of passwords: " << thePasswords.size << "<hr>Passwords string: " << thePasswords;
  UserCalculator currentUser;
  currentUser.currentTable=DatabaseStrings::usersTableName;
  bool result=true;
  bool doSendEmails=false;
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
      if (!currentUser.FetchOneUserRow(*this, &comments))
        result=false;
    currentUser.SetColumnEntry("userRole", userRole, *this, &comments);
    if (thePasswords.size==0 || thePasswords.size!=theEmails.size)
    { currentUser.ComputeActivationToken();
      if (!currentUser.SetColumnEntry("activationToken", currentUser.actualActivationToken.value, *this, &comments))
      { comments << "Setting activation token failed.";
        result=false;
        currentUser.actualActivationToken="";
      }
    } else
    { currentUser.enteredPassword=thePasswords[i];
      //stOutput << "Debug: user:<br>" << currentUser.username.value << "<br>password:<br>"
      //<< CGI::StringToURLString(thePasswords[i]) << "<br>";
      if (!currentUser.SetPassword(*this, &comments))
        result=false;
      currentUser.SetColumnEntry("activationToken", "activated", *this, &comments);
    }
  }
  for (int i=0; i<theEmails.size; i++)
  { currentUser.username=theEmails[i];
    //if (!currentUser.IamPresentInTable(*this, currentFileUsersTableName))
      //if (!this->InsertRow(DatabaseStrings::userColumnLabel, theEmails[i], currentFileUsersTableName, comments))
        //result=false;
    if (!currentUser.SetColumnEntry(DatabaseStrings::userGroupLabel, userGroup, *this, &comments))
      result=false;
  }
  if (!result)
    comments << "<br>Failed to create all users. ";
  if (doSendEmails)
  { doSendEmails=false;
    comments << "Sending actual emails disabled for security reasons (system not stable enough yet). ";
  }
  if (doSendEmails)
    if (!this->SendActivationEmail(theEmails, comments))
      result=false;
  return result;
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
  if (!this->FetchOneUserRow(theRoutines, &comments))
    return false;
  this->actualActivationToken= this->GetSelectedRowEntry("activationToken");
  if (this->actualActivationToken.value=="")
  { comments << "Failed to fetch activation token for user: " << this->username.value;
    return false;
  }
  if (this->actualActivationToken=="activated")
  { comments << "Account of user: " << this->username.value << "already activated";
    return false;
  }
  output= this->GetActivationAddressFromActivationToken
  (this->actualActivationToken.value, calculatorBase, this->username.value);
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
  if (theUser.username.value==theGlobalVariables.userDefault.username.value)
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
    if (!theUser.FetchOneColumn(DatabaseStrings::userColumnLabel, recommendedNewName, theRoutines, &out))
      return out.str();
    theUser.username.value=recommendedNewName;
  }
  if (theUser.username.value!=inputUsernameUnsafe)
  { out << "<br>Username is CaSe SeNsItiVE and consists of the full email address: perhaps you meant: <br>"
    << theUser.username.value << "<br>";
    //out << "<hr>" << Crasher::GetStackTraceEtcErrorMessage() << "<hr>";
  }
  return out.str();
}
#endif //MACRO_use_MySQL


bool DatabaseRoutinesGlobalFunctions::LoginViaDatabase
(UserCalculatorData& theUseR, std::stringstream* comments)
{ (void) comments;
  (void) theUseR;
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::LoginViaDatabase");
  DatabaseRoutines theRoutines;
  UserCalculator userWrapper;
  userWrapper.::UserCalculatorData::operator=(theUseR);
  //stOutput << "Logging in: " << userWrapper.ToStringUnsecure();
  userWrapper.currentTable=DatabaseStrings::usersTableName;
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
    << CGI::StringToURLString(tableNames[i]) << "&" << theGlobalVariables.ToStringCalcArgsNoNavigation(true);
    out << "<tr><td><a href=\"" << linkStream.str() << "\">" << CGI::URLStringToNormal(tableNames[i], false)
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
      output[i][j]=CGI::URLStringToNormal(theQuery.allQueryResultStrings[i][j], false);
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
      outputColumnLabels[i]= CGI::URLStringToNormal(theFieldQuery.allQueryResultStrings[i][0], false);
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
  //stOutput << "<hr>DEBUG: fetch entry: firing up query: " << queryStream.str() << "<hr>";
  DatabaseQuery theQuery(*this, queryStream.str(), failureComments);
  outputUnsafe="";
  if (!theQuery.flagQuerySucceeded)
  { if (failureComments!=0)
      *failureComments << "<b>Query failed - column may not exist (or some other error occurred). </b>";
  //stOutput << "<hr><b>Query failed - column may not exist (or some other error occurred). </b><hr>";
    return false;
  }
  if (!theQuery.flagQueryReturnedResult)
  { if (failureComments!=0)
      *failureComments << "<b>Query did not return a result - column may not exist. </b>";
  //stOutput << "<hr>DEBUG: Query did not return a result - column may not exist. <hr>";
    return false;
  }
  outputUnsafe= CGI::URLStringToNormal(theQuery.firstResultString, false);
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
  //stOutput << "DEBUG: firing: query: " << theQuery.theQueryString;
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
  << ".users(username, " << DatabaseStrings::deadlinesIdColumnName << ", "
  << DatabaseStrings::problemWeightsIdColumnName << ")"
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
  std::stringstream tableCols, deadlineTableCols, probWeightTableCols;
  tableCols
  << "id int NOT NULL AUTO_INCREMENT PRIMARY KEY, "
  << "username VARCHAR(255) NOT NULL, "
  << "password LONGTEXT, "
  << "email LONGTEXT, "
  << "authenticationCreationTime LONGTEXT, "
  << "authenticationToken LONGTEXT, "
  << "activationToken LONGTEXT, "
  << "userRole LONGTEXT, "
  << "userInfo LONGTEXT, "
  << DatabaseStrings::problemWeightsIdColumnName << " LONGTEXT, "
  << DatabaseStrings::deadlinesIdColumnName << " LONGTEXT, "
  << DatabaseStrings::sectionsTaughtByUser << " LONGTEXT, "
  << "problemData LONGTEXT "
  ;
  if (! this->CreateTable
      (DatabaseStrings::usersTableName, tableCols.str(), commentsOnFailure, outputfirstLogin))
    return false;
  deadlineTableCols << DatabaseStrings::deadlinesIdColumnName
  << " VARCHAR(50) not null, " << DatabaseStrings::infoColumnInDeadlinesTable << " LONGTEXT";
  if (!this->CreateTable
      (DatabaseStrings::deadlinesTableName,
      deadlineTableCols.str(), commentsOnFailure, 0))
    return false;
  probWeightTableCols << DatabaseStrings::problemWeightsIdColumnName
  << " VARCHAR(50) not null, " << DatabaseStrings::infoColumnInProblemWeightsTable << " LONGTEXT";
  if(!this->CreateTable
    (DatabaseStrings::problemWeightsTableName, probWeightTableCols.str(), commentsOnFailure, 0))
    return false;
  return true;
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
