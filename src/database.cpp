// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "database.h"
#include "crypto.h"
#include "webserver.h"
#include "system_functions_global_objects.h"
#include "database.h"
#include "calculator_database_mongo.h"
#include "calculator_problem_storage.h"
#include "string_constants.h"

static ProjectInformationInstance projectInfoDatabaseCPP(__FILE__, "Database-related code. ");

bool Database::User::SetPassword(
  const std::string& inputUsername,
  const std::string& inputNewPassword,
  std::string& outputAuthenticationToken,
  std::stringstream& comments
) {
  if (!global.flagDatabaseCompiled) {
    comments << "Database not present. ";
    return false;
  }
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::SetPassword");
  if (!global.flagLoggedIn) {
    comments << "Changing passwords allowed for logged-in users only. ";
    return false;
  }
  UserCalculator theUser;
  theUser.username = inputUsername;
  theUser.enteredPassword = inputNewPassword;
  bool result = theUser.SetPassword(&comments);
  theUser.ResetAuthenticationToken(&comments);
  outputAuthenticationToken = theUser.actualAuthenticationToken;
  return result;
}

bool Database::User::UserExists(
  const std::string& inputUsername, std::stringstream& comments
) {
  if (!global.flagDatabaseCompiled) {
    comments << "No database available.";
    return false;
  }
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::UserExists");
  if (!global.flagLoggedIn) {
    return false;
  }
  JSData theUserQuery;
  theUserQuery[DatabaseStrings::labelUsername] = inputUsername;
  List<JSData> theUsers;
  this->owner->FindFromJSON(
    DatabaseStrings::tableUsers, theUserQuery, theUsers, - 1, nullptr, &comments
  );
  return theUsers.size > 0;
}

bool Database::User::UserDefaultHasInstructorRights() {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::UserDefaultHasInstructorRights");
  if (global.UserDefaultHasAdminRights())
    return true;
  if (!global.flagDatabaseCompiled) {
    return true;
  }
  if (!global.flagLoggedIn) {
    return false;
  }
  return
    global.userDefault.userRole == UserCalculatorData::Roles::admin ||
    global.userDefault.userRole == UserCalculator::Roles::instructor ||
    global.userDefault.userRole == UserCalculator::Roles::teacher;
}

bool Database::User::LogoutViaDatabase() {
  if (!global.flagDatabaseCompiled) {
    return true;
  }
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::LogoutViaDatabase");
  if (!global.flagLoggedIn) {
    return true;
  }
  UserCalculator theUser;
  theUser.UserCalculatorData::operator=(global.userDefault);
  theUser.ResetAuthenticationToken(nullptr);
  global.SetWebInpuT("authenticationToken", "");
  global.CookiesToSetUsingHeaders.SetKeyValue("authenticationToken", "");
  return true;
}

QueryExact::QueryExact() {
}

QueryExact::QueryExact(
  const std::string& desiredCollection,
  const std::string& label,
  const std::string& desiredValue
) {
  this->collection = desiredCollection;
  this->nestedLabels.AddOnTop(label);
  this->value = desiredValue;
}

QueryExact::QueryExact(
  const std::string& desiredCollection,
  const List<std::string>& desiredLabels,
  const std::string& desiredValue
) {
  this->collection = desiredCollection;
  this->value = desiredValue;
  this->nestedLabels = desiredLabels;
}

bool QueryExact::isEmpty() const {
  if (this->collection == "") {
    return true;
  }
  if (this->nestedLabels.size == 0) {
    return true;
  }
  return false;
}

JSData QueryExact::ToJSON() const {
  JSData result;
  result.theType = JSData::token::tokenObject;
  result[this->getLabel()] = this->value;
  return result;
}

std::string QueryExact::getLabel() const {
  std::stringstream out;
  for (int i = 0; i < this->nestedLabels.size; i ++) {
    out << JSData::EncodeKeyForMongo(this->nestedLabels[i]);
    if (i != this->nestedLabels.size - 1) {
      out << ".";
    }
  }
  return out.str();
}

std::string QueryExact::getCollectionAndLabel() const {
  std::stringstream out;
  out << this->collection << "." << this->getLabel();
  return out.str();
}

void QueryExact::SetLabelValue(const std::string& label, const std::string& desiredValue) {
  this->value = desiredValue;
  this->nestedLabels.SetSize(1);
  this->nestedLabels[0] = label;
}

bool Database::FindOne(const QueryExact& query, JSData& output, std::stringstream* commentsOnFailure) {
  if (global.flagDatabaseCompiled) {
    return this->mongoDB.FindOneFromQueryString(query.collection, query.ToJSON().ToString(false), output, commentsOnFailure);
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Database::FindOne not implemented yet.";
  }
  return false;
}

bool Database::UpdateOne(
  const QueryExact& findQuery,
  const JSData& dataToMerge,
  std::stringstream* commentsOnFailure
) {
  if (findQuery.isEmpty()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty query not allowed";
    }
    return false;
  }
  if (global.flagDatabaseCompiled) {
    return this->mongoDB.UpdateOne(findQuery, dataToMerge, commentsOnFailure);
  } else if (global.flagDatabaseUseFallback) {
    return this->theFallBack.UpdateOne(findQuery, dataToMerge, commentsOnFailure);
  }

  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "UpdateOne fail: no DB compiled and fallback is disabled. ";
  }
  return false;
}

bool Database::FindOneFromSome(
  const List<QueryExact>& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  if (global.flagDatabaseCompiled) {
    return this->mongoDB.FindOneFromSome(findOrQueries, output, commentsOnFailure);
  } else if (global.flagDatabaseUseFallback) {
    return this->theFallBack.FindOneFromSome(findOrQueries, output, commentsOnFailure);
  } else {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Find one from some failed: fallback database disabled. ";
    }
    return false;
  }
}


void Database::CreateHashIndex(
  const std::string& collectionName, const std::string& theKey
) {
  if (global.flagDatabaseCompiled) {
    this->mongoDB.CreateHashIndex(collectionName, theKey);
  } else if (global.flagDatabaseUseFallback) {
    this->theFallBack.CreateHashIndex(collectionName, theKey);
  }
}

bool Database::initializeWorker() {
  MacroRegisterFunctionWithName("Database::initializeWorker");
  if (this->flagInitializedWorker) {
    return true;
  }
  if (global.flagDatabaseCompiled) {
    this->mongoDB.initialize();
  }
  this->CreateHashIndex(DatabaseStrings::tableUsers, DatabaseStrings::labelUsername);
  this->CreateHashIndex(DatabaseStrings::tableUsers, DatabaseStrings::labelEmail);
  this->CreateHashIndex(DatabaseStrings::tableUsers, DatabaseStrings::labelInstructor);
  this->CreateHashIndex(DatabaseStrings::tableUsers, DatabaseStrings::labelUserRole);
  this->CreateHashIndex(DatabaseStrings::tableProblemInformation, DatabaseStrings::labelProblemName);
  this->CreateHashIndex(DatabaseStrings::tableDeleted, DatabaseStrings::labelUsername);
  this->flagInitializedWorker = true;
  return true;
}

bool Database::initializeServer() {
  MacroRegisterFunctionWithName("Database::initializeServer");
  this->theUser.owner = this;
  this->theFallBack.owner = this;
  this->flagInitializedServer = true;
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    return true;
  }
  if (global.flagDatabaseCompiled) {
    return true;
  }
  global.flagDatabaseUseFallback = true;
  global << logger::red << "Calculator compiled without (mongoDB) database support. "
  << logger::green << "Using " << logger::red
  << "**SLOW** " << logger::green << "fall-back JSON storage." << logger::endL;
  this->theFallBack.initialize();
  return true;
}

void GlobalVariables::initModifiableDatabaseFields() {
  MacroRegisterFunctionWithName("GlobalVariables::initModifiableDatabaseFields");
  List<List<std::string> >& modifiableData = global.databaseModifiableFields;
  List<std::string> currentEntry;
  modifiableData.Reserve(10);
  currentEntry.AddOnTop(DatabaseStrings::tableUsers);
  currentEntry.AddOnTop(DatabaseStrings::objectSelectoR);
  currentEntry.AddOnTop(DatabaseStrings::labelProblemDataJSON);
  currentEntry.AddOnTop(DatabaseStrings::anyFielD);
  modifiableData.AddOnTop(currentEntry);
  currentEntry.SetSize(0);
  currentEntry.AddOnTop(DatabaseStrings::tableUsers);
  currentEntry.AddOnTop(DatabaseStrings::objectSelectoR);
  currentEntry.AddOnTop(DatabaseStrings::labelProblemDatA);
  modifiableData.AddOnTop(currentEntry);
  currentEntry.SetSize(0);
  currentEntry.AddOnTop(DatabaseStrings::tableEmailInfo);
  currentEntry.AddOnTop(DatabaseStrings::objectSelectoR);
  modifiableData.AddOnTop(currentEntry);
  std::fstream outputFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual(
    outputFile, "/calculator-html/modifiable_database_fields.js", false, true, false
  );

  JSData modifiableDataJSON;
  modifiableDataJSON = modifiableData;
  JSData toWrite;
  toWrite["modifiableFields"] = modifiableDataJSON;
  outputFile << "//File automatically generated. Please do not modify.\n";
  outputFile << "\"use strict\";\n";
  outputFile << "var modifiableDatabaseData = " << toWrite.ToString(false, true) << ";\n";
  outputFile << "module.exports = {modifiableDatabaseData};";
}

ProblemData::ProblemData() {
  this->randomSeed = 0;
  this->flagRandomSeedGiven = false;
  this->numCorrectlyAnswered = 0;
  this->totalNumSubmissions = 0;
  this->expectedNumberOfAnswersFromDB = 0;
  this->knownNumberOfAnswersFromHD = - 1;
  this->flagProblemWeightIsOK = false;
}

void ProblemData::AddEmptyAnswerIdOnTop(const std::string& inputAnswerId) {
  Answer theAnswer;
  theAnswer.answerId = inputAnswerId;
  this->theAnswers.SetKeyValue(inputAnswerId, theAnswer);
}

std::string ProblemData::ToStringAvailableAnswerIds() {
  std::stringstream out;
  out << "Available answer ids: ";
  for (int i = 0; i < this->theAnswers.size(); i ++) {
    out << this->theAnswers.theValues[i].answerId;
    if (i != this->theAnswers.size() - 1) {
      out << ", ";
    }
  }
  return out.str();
}

bool ProblemDataAdministrative::GetWeightFromCoursE(
  const std::string& theCourseNonURLed, Rational& output, std::string* outputAsGivenByInstructor
) {
  MacroRegisterFunctionWithName("ProblemDataAdministrative::GetWeightFromSection");
  if (!this->problemWeightsPerCoursE.Contains(theCourseNonURLed)) {
    return false;
  }
  std::string tempString;
  if (outputAsGivenByInstructor == nullptr) {
    outputAsGivenByInstructor = &tempString;
  }
  *outputAsGivenByInstructor = this->problemWeightsPerCoursE.GetValueCreate(theCourseNonURLed);
  return output.AssignStringFailureAllowed(*outputAsGivenByInstructor);
}

std::string ProblemDataAdministrative::ToString() const {
  std::stringstream out;
  out << this->deadlinesPerSection.ToStringHtml();
  out << this->problemWeightsPerCoursE.ToStringHtml();
  return out.str();
}

bool ProblemData::CheckConsistency() const {
  MacroRegisterFunctionWithName("ProblemData::CheckConsistency");
  for (int i = 0; i < this->theAnswers.size(); i ++) {
    if (StringRoutines::StringTrimWhiteSpace(this->theAnswers.theValues[i].answerId) == "") {
      global.fatal << "This is not supposed to happen: empty answer id." << global.fatal;
    }
  }
  return true;
}

bool ProblemData::CheckConsistencyMQids() const {
  MacroRegisterFunctionWithName("ProblemData::CheckConsistencyMQids");
  for (int i = 0; i < this->theAnswers.size(); i ++) {
    if (StringRoutines::StringTrimWhiteSpace(this->theAnswers.theValues[i].idMQfielD) == "") {
      std::stringstream errorStream;
      errorStream << "This is not supposed to happen: empty idMQfield. The answer id is: "
      << this->theAnswers.theValues[i].answerId << "<br>" << this->ToString() << "<hr>Answer information: "
      << this->ToString() << "<br>";
      global.fatal << errorStream.str() << global.fatal;
    }
  }
  return true;
}

std::string ProblemData::ToString() const {
  std::stringstream out;
  out << "Problem data. "
  << "Random seed: " << this->randomSeed;
  if (this->flagRandomSeedGiven) {
    out << " (given)";
  }
  out << ". <br>";
  for (int i = 0; i < this->theAnswers.size(); i ++) {
    Answer& currentA = this->theAnswers.theValues[i];
    out << "AnswerId: " << currentA.answerId;
    out << ", numCorrectSubmissions: " << currentA.numCorrectSubmissions;
    out << ", numSubmissions: " << currentA.numSubmissions;
    out << ", firstCorrectAnswer: ";
    if (currentA.firstCorrectAnswerClean == "") {
      out << "[none yet], ";
    } else {
      out << "[" << currentA.firstCorrectAnswerClean << "], ";
    }
    out << "<br>";
  }
  out << "Admin data: " << this->adminData.ToString();
  return out.str();
}

UserCalculator::UserCalculator() {
  this->flagNewAuthenticationTokenComputedUserNeedsIt = false;
  this->approximateHoursSinceLastTokenWasIssued = 2;
}

UserCalculator::~UserCalculator() {
  for (unsigned i = 0; i < this->enteredPassword.size(); i ++) {
    this->enteredPassword[i] = ' ';
  }
  for (unsigned i = 0; i < this->usernameHashedPlusPassWordHashed.size(); i ++) {
    this->usernameHashedPlusPassWordHashed[i] = ' ';
  }
  for (unsigned i = 0; i < this->enteredHashedSaltedPassword.size(); i ++) {
    this->enteredHashedSaltedPassword[i] = ' ';
  }
  for (unsigned i = 0; i < this->actualHashedSaltedPassword.size(); i ++) {
    this->actualHashedSaltedPassword[i] = ' ';
  }
}

std::string UserCalculator::ToStringSelectedColumns() {
  MacroRegisterFunctionWithName("DatabaseRoutines::ToStringSelectedColumns");
  std::stringstream out;
  out << this->selectedColumnsUnsafe.size << " columns selected. ";
  for (int i = 0; i < this->selectedColumnsUnsafe.size; i ++) {
    out << "<br>" << this->selectedColumnsUnsafe[i] << ": "
    << this->selectedColumnsUnsafe[i] << this->selectedColumnsRetrievalFailureRemarks[i];
  }
  return out.str();
}

std::string UserCalculator::ToString() {
  MacroRegisterFunctionWithName("UserCalculator::ToString");
  std::stringstream out;

  out << "Calculator user: " << this->username
  << "<br>Section: computed: " << this->sectionComputed
  << ", in DB: " << this->sectionInDB
  << "<br>Sections taught: " << this->sectionsTaught.ToStringCommaDelimited()
  << "<br>Course: computed: " << this->courseInDB << ", in DB: " << this->courseComputed;

  Rational weightRat;
  for (int i = 0; i < this->theProblemData.size(); i ++) {
    out << "<br>Problem: " << this->theProblemData.theKeys[i] << "; random seed: "
    << this->theProblemData.theValues[i].randomSeed << "; numSubmissions: "
    << this->theProblemData.theValues[i].totalNumSubmissions
    << "; correct: "
    << this->theProblemData.theValues[i].numCorrectlyAnswered
    << "; points: "
    << this->theProblemData.theValues[i].Points
    << ";";
    if (!this->theProblemData.theValues[i].adminData.GetWeightFromCoursE(this->courseComputed, weightRat)) {
      out << " (weight not available). ";
    } else {
      out << " weight: " << weightRat.ToString();
    }
  }
  out << "<br>Deadline info: "
  << HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(this->deadlines.ToString(false));
  return out.str();
}

bool UserCalculator::FetchOneColumn(
  const std::string& columnNameUnsafe,
  std::string& outputUnsafe,
  std::stringstream* failureComments
) {
  MacroRegisterFunctionWithName("UserCalculator::FetchOneColumn");
  (void) columnNameUnsafe;
  (void) outputUnsafe;
  (void) failureComments;
  global.fatal << "not implemented" << global.fatal;
  return false;
}

bool UserCalculator::AuthenticateWithToken(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("UserCalculator::AuthenticateWithToken");
  (void) commentsOnFailure;
  if (this->enteredAuthenticationToken == "") {
    return false;
  }
  TimeWrapper now;
  now.AssignLocalTime();
  this->approximateHoursSinceLastTokenWasIssued =
  now.SubtractAnotherTimeFromMeAndGet_APPROXIMATE_ResultInHours(this->authenticationCreationTime);
  //<-to do: revoke authentication token if expired.
  return this->enteredAuthenticationToken == this->actualAuthenticationToken;
}

bool UserCalculator::LoadFromDB(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral) {
  MacroRegisterFunctionWithName("UserCalculator::FetchOneUserRow");
  (void) commentsGeneral;
  double startTime = global.GetElapsedSeconds();
  if (!Database::get().theUser.LoadUserInfo(*this, commentsOnFailure)) {
    return false;
  }
  this->ComputeCourseInfo();
  if (this->deadlineSchema != "") {
    QueryExact findDeadlines(DatabaseStrings::tableDeadlines, DatabaseStrings::labelDeadlinesSchema, this->deadlineSchema);
    JSData outDeadlinesQuery;
    if (Database::get().FindOne(
      findDeadlines,
      outDeadlinesQuery,
      commentsGeneral
    )) {
      this->deadlines = outDeadlinesQuery[DatabaseStrings::labelDeadlines];
    }
  }
  if (this->problemWeightSchema != "") {
    QueryExact findProblemWeights(DatabaseStrings::tableProblemWeights, DatabaseStrings::labelProblemWeightsSchema, this->problemWeightSchema);
    JSData outProblemWeightsQuery;
    if (Database::get().FindOne(
      findProblemWeights, outProblemWeightsQuery, commentsGeneral
    )) {
      this->problemWeights = outProblemWeightsQuery[DatabaseStrings::labelProblemWeights];
    }
  }
  global.timeStats["userLoadTime"] = global.GetElapsedSeconds() - startTime;
  return true;
}

bool UserCalculatorData::LoadFromJSON(JSData& input) {
  MacroRegisterFunctionWithName("UserCalculatorData::LoadFromJSON");
  this->userId                            = input[DatabaseStrings::labelUserId                            ].theString;
  this->username                          = input[DatabaseStrings::labelUsername                          ].theString;
  this->email                             = input[DatabaseStrings::labelEmail                             ].theString;
  this->actualActivationToken             = input[DatabaseStrings::labelActivationToken                   ].theString;
  this->timeOfActivationTokenCreation     = input[DatabaseStrings::labelTimeOfActivationTokenCreation     ].theString;
  this->actualAuthenticationToken         = input[DatabaseStrings::labelAuthenticationToken               ].theString;
  this->timeOfAuthenticationTokenCreation = input[DatabaseStrings::labelTimeOfAuthenticationTokenCreation ].theString;
  this->problemDataStrinG                 = input[DatabaseStrings::labelProblemDatA                       ].theString;
  this->problemDataJSON                   = input[DatabaseStrings::labelProblemDataJSON                   ]       ;
  this->actualHashedSaltedPassword        = input[DatabaseStrings::labelPassword                          ].theString;
  this->userRole                          = input[DatabaseStrings::labelUserRole                          ].theString;

  this->instructorInDB                    = input[DatabaseStrings::labelInstructor                        ].theString;
  this->semesterInDB                      = input[DatabaseStrings::labelSemester                          ].theString;
  this->sectionInDB                       = input[DatabaseStrings::labelSection                           ].theString;
  this->courseInDB                        = input[DatabaseStrings::labelCurrentCourses                    ].theString;
  this->sectionsTaught.SetSize(0);
  JSData sectionsTaughtList = input[DatabaseStrings::labelSectionsTaught];
  if (sectionsTaughtList.theType == JSData::token::tokenArray) {
    for (int i = 0; i < sectionsTaughtList.theList.size; i ++) {
      this->sectionsTaught.AddOnTop(sectionsTaughtList.theList[i].theString);
    }
  }
  return true;
}

JSData UserCalculatorData::ToJSON() {
  MacroRegisterFunctionWithName("UserCalculatorData::ToJSON");
  JSData result;
  result[DatabaseStrings::labelUserId                            ] = this->userId                                ;
  result[DatabaseStrings::labelUsername                          ] = this->username                              ;
  result[DatabaseStrings::labelEmail                             ] = this->email                                 ;
  result[DatabaseStrings::labelActivationToken                   ] = this->actualActivationToken                 ;
  result[DatabaseStrings::labelTimeOfActivationTokenCreation     ] = this->timeOfActivationTokenCreation         ;
  result[DatabaseStrings::labelAuthenticationToken               ] = this->actualAuthenticationToken             ;
  result[DatabaseStrings::labelTimeOfAuthenticationTokenCreation ] = this->timeOfAuthenticationTokenCreation     ;
  if (this->problemDataJSON.objects.size() != 0) {
    result[DatabaseStrings::labelProblemDatA                     ] = this->problemDataStrinG                     ;
  }
  result[DatabaseStrings::labelProblemDataJSON                   ] = this->problemDataJSON                       ;
  if (this->problemDataJSON.theType == JSData::token::tokenUndefined) {
    result[DatabaseStrings::labelProblemDataJSON].theType = JSData::token::tokenArray;
    result[DatabaseStrings::labelProblemDataJSON].theList.SetSize(0);
  }
  result[DatabaseStrings::labelPassword                          ] = this->actualHashedSaltedPassword            ;
  result[DatabaseStrings::labelUserRole                          ] = this->userRole                              ;

  result[DatabaseStrings::labelInstructor                        ] = this->instructorInDB                        ;
  result[DatabaseStrings::labelSemester                          ] = this->semesterInDB                          ;
  result[DatabaseStrings::labelSection                           ] = this->sectionInDB                           ;
  result[DatabaseStrings::labelCurrentCourses                    ] = this->courseInDB                            ;
  JSData sectionsTaughtList;
  sectionsTaughtList.theType = JSData::token::tokenArray;
  for (int i = 0; i < this->sectionsTaught.size; i ++) {
    sectionsTaughtList[i] = this->sectionsTaught[i];
  }
  result[DatabaseStrings::labelSectionsTaught] = sectionsTaughtList;
  for (int i = result.objects.size() - 1; i >= 0; i --) {
    JSData& currentValue = result.objects.theValues[i];
    if (currentValue.theString == "" && currentValue.theType == JSData::token::tokenString) {
      result.objects.RemoveKey(result.objects.theKeys[i]);
    }
  }
  return result;
}

bool UserCalculatorData::ComputeCourseInfo() {
  MacroRegisterFunctionWithName("UserCalculator::ComputeCourseInfo");
  bool isAdmin = (this->userRole == UserCalculatorData::Roles::admin && this->username == global.userDefault.username);
  if (
    global.UserStudentVieWOn() &&
    isAdmin &&
    global.GetWebInput("studentSection") != ""
  ) {
    //<- warning, the user may not be
    //fully logged-in yet so global.UserDefaultHasAdminRights()
    //does not work right.
    this->sectionComputed = HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("studentSection"), false);
  } else {
    this->sectionComputed = this->sectionInDB;
  }
  if (isAdmin && global.GetWebInput("courseHome") != "") {
    this->courseComputed =
    HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("courseHome"), false);
  } else {
    this->courseComputed = this->courseInDB;
  }
  if (isAdmin) {
    this->instructorComputed = this->username;
  } else {
    this->instructorComputed = this->instructorInDB;
  }
  this->deadlineSchema = "deadlines" + this->instructorComputed + this->courseComputed + this->semesterComputed;
  this->problemWeightSchema = "problemWeights" + this->instructorComputed;
  return true;
}

void UserCalculator::SetProblemData(const std::string& problemName, const ProblemData& inputData) {
  MacroRegisterFunctionWithName("UserCalculator::SetProblemData");
  this->theProblemData.SetKeyValue(problemName, inputData);
}

std::string ProblemData::StorE(){
  MacroRegisterFunctionWithName("ProblemData::StorE");
  std::stringstream out;
  if (this->flagRandomSeedGiven) {
    out << "randomSeed=" << this->randomSeed;
  }
  for (int i = 0; i < this->theAnswers.size(); i ++) {
    Answer& currentA = this->theAnswers.theValues[i];
    if (this->flagRandomSeedGiven || i != 0) {
      out << "&";
    }
    out << HtmlRoutines::ConvertStringToURLString(currentA.answerId, false) << "=";
    std::stringstream questionsStream;
    questionsStream
    << "numCorrectSubmissions =" << currentA.numCorrectSubmissions
    << "&numSubmissions =" << currentA.numSubmissions
    << "&firstCorrectAnswer =" << HtmlRoutines::ConvertStringToURLString(currentA.firstCorrectAnswerClean, false);
    out << HtmlRoutines::ConvertStringToURLString(questionsStream.str(), false);
  }
  return out.str();
}

JSData ProblemData::StoreJSON() {
  MacroRegisterFunctionWithName("ProblemData::StoreJSON");
  JSData result;
  result.theType = JSData::token::tokenObject;
  if (this->flagRandomSeedGiven) {
    result["randomSeed"] = std::to_string(this->randomSeed);
  }
  for (int i = 0; i < this->theAnswers.size(); i ++) {
    Answer& currentA = this->theAnswers.theValues[i];
    JSData currentAnswerJSON;
    currentAnswerJSON["numCorrectSubmissions"] = std::to_string(currentA.numCorrectSubmissions);
    currentAnswerJSON["numSubmissions"] = std::to_string(currentA.numSubmissions);
    currentAnswerJSON["firstCorrectAnswer"] = HtmlRoutines::ConvertStringToURLString(currentA.firstCorrectAnswerClean, false);
    result[HtmlRoutines::ConvertStringToURLString(currentA.answerId, false)] = currentAnswerJSON;
  }
  return result;
}

bool UserCalculator::ShouldCommentOnMissingUser() {
  if (this->username.size() < 4) {
    return true;
  }
  return
  this->username == "default" ||
  this->username == "admin" ||
  this->username == "guest" ||
  this->username == "student" ||
  this->username == "teacher";
}

std::string UserCalculator::FirstLoginMessage() {
  std::stringstream out;
  if (global.flagRequestComingLocally) {
    out << "If this is your first run, set the username to "
    << "admin and enter the password you desire. "
    << "The password will then be automatically set. "
    << "To add further accounts login as admin and go to 'Accounts'. ";
  }
  return out.str();
}

bool UserCalculator::Authenticate(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  std::stringstream secondCommentsStream;
  if (!this->LoadFromDB(&secondCommentsStream)) {
    if (commentsOnFailure != nullptr && this->ShouldCommentOnMissingUser()) {
      *commentsOnFailure << "User " << this->username << " does not exist. ";
      *commentsOnFailure << this->FirstLoginMessage();
    }
    return false;
  }
  if (this->AuthenticateWithToken(&secondCommentsStream)) {
    return true;
  }
  bool result = this->AuthenticateWithUserNameAndPass(commentsOnFailure);
  if (this->enteredPassword != "") {
    this->ResetAuthenticationToken(commentsOnFailure);
  }
  //<- this needs to be fixed: an attacker may cause denial of service by launching fake login attempts.
  return result;
}

bool UserCalculator::AuthenticateWithUserNameAndPass(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  (void) commentsOnFailure;
  this->ComputeHashedSaltedPassword();
  return this->enteredHashedSaltedPassword == this->actualHashedSaltedPassword;
}

bool UserCalculator::ResetAuthenticationToken(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("UserCalculator::ResetAuthenticationToken");
  TimeWrapper now;
  now.AssignLocalTime();
  std::stringstream out;
  out << now.theTimeStringNonReadable << rand();
  this->actualAuthenticationToken = Crypto::computeSha3_256OutputBase64URL(out.str());
  QueryExact findUser(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelUsername,
    this->username
  );
  JSData setUser;
  setUser[DatabaseStrings::labelAuthenticationToken] = this->actualAuthenticationToken;
  setUser[DatabaseStrings::labelTimeOfAuthenticationTokenCreation] = now.theTimeStringNonReadable;
  Database::get().UpdateOne(
    findUser, setUser, commentsOnFailure
  );
  this->flagNewAuthenticationTokenComputedUserNeedsIt = true;
  return true;
}

bool UserCalculator::SetPassword(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("UserCalculator::SetPassword");
  if (this->enteredPassword == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty password not allowed. ";
    }
    return false;
  }
  this->ComputeHashedSaltedPassword();
  QueryExact findUser(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelUsername,
    this->username
  );
  JSData setUser;
  setUser[DatabaseStrings::labelPassword] = this->enteredHashedSaltedPassword;
  return Database::get().UpdateOne(
    findUser, setUser, commentsOnFailure
  );
}

bool UserCalculator::IsAcceptableCharDatabaseInpuT(char theChar) {
  if (MathRoutines::isADigit(theChar)) {
    return true;
  }
  if (MathRoutines::isALatinLetter(theChar)) {
    return true;
  }
  switch (theChar) {
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

bool UserCalculator::IsAcceptableDatabaseInpuT(const std::string& input, std::stringstream* comments) {
  MacroRegisterFunctionWithName("UserCalculator::IsAcceptableDatabaseInput");
  for (unsigned i = 0; i < input.size(); i ++) {
    if (!UserCalculator::IsAcceptableCharDatabaseInpuT(input[i])) {
      if (comments != nullptr) {
        *comments << "Input: " << input << " contains at least one invalid character: " << input[i] << ".";
      }
      return false;
    }
  }
  return true;
}

std::string UserCalculator::GetSelectedRowEntry(const std::string& theKey) {
  MacroRegisterFunctionWithName("UserCalculator::GetSelectedRowEntry");
  int theIndex = this->selectedRowFieldNamesUnsafe.GetIndex(theKey);
  if (theIndex == - 1) {
    return "";
  }
  return this->selectedRowFieldsUnsafe[theIndex];
}

bool Database::User::SendActivationEmail(
  const std::string& emailList,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsGeneralSensitive
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::SendActivationEmail");
  List<std::string> theEmails;
  StringRoutines::StringSplitDefaultDelimiters(emailList, theEmails);
  return Database::User::SendActivationEmail(
    theEmails, commentsOnFailure, commentsGeneral, commentsGeneralSensitive
  );
}

bool Database::User::SendActivationEmail(
  const List<std::string>& theEmails,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsGeneralSensitive
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::SendActivationEmail");
  UserCalculator currentUser;
  bool result = true;
  for (int i = 0; i < theEmails.size; i ++) {
    global << "Sending activation email, user "
    << i + 1 << " out of " << theEmails.size << " ... ";
    currentUser.username = theEmails[i];
    currentUser.email = theEmails[i];
    global.server().GetActiveWorker().DoSetEmail(
      currentUser, commentsOnFailure, commentsGeneral, commentsGeneralSensitive
    );
  }
  if (commentsOnFailure != nullptr) {
    global << commentsOnFailure->str();
  }
  if (commentsGeneral != nullptr && commentsOnFailure != commentsGeneral) {
    global << commentsGeneral->str();
  }
  if (commentsGeneralSensitive != nullptr && commentsGeneralSensitive != commentsOnFailure) {
    global << commentsGeneralSensitive->str();
  }
  return result;
}

ProblemData& UserCalculator::GetProblemDataAddIfNotPresent(const std::string& problemName) {
  MacroRegisterFunctionWithName("UserCalculator::GetProblemDataAddIfNotPresent");
  return this->theProblemData.GetValueCreate(problemName);
}

bool ProblemData::LoadFroM(const std::string& inputData, std::stringstream& commentsOnFailure) {
  MacroRegisterFunctionWithName("ProblemData::LoadFrom");
  MapList<std::string, std::string, MathRoutines::HashString> theMap;
  if (!HtmlRoutines::ChopCGIString(inputData, theMap, commentsOnFailure)) {
    return false;
  }
  this->Points = 0;
  this->numCorrectlyAnswered = 0;
  this->totalNumSubmissions = 0;
  this->flagRandomSeedGiven = false;
  if (global.UserRequestRequiresLoadingRealExamData()) {
    if (theMap.Contains("randomSeed")) {
      this->randomSeed = static_cast<unsigned>(atoi(theMap.GetValueCreate("randomSeed").c_str()));
      this->flagRandomSeedGiven = true;
    }
  }
  this->theAnswers.Clear();
  bool result = true;
  MapList<std::string, std::string, MathRoutines::HashString> currentQuestionMap;
  for (int i = 0; i < theMap.size(); i ++) {
    if (theMap.theKeys[i] == "randomSeed") {
      continue;
    }
    this->AddEmptyAnswerIdOnTop(HtmlRoutines::ConvertURLStringToNormal(theMap.theKeys[i], false));
    Answer& currentA = *this->theAnswers.theValues.LastObject();
    std::string currentQuestion = HtmlRoutines::ConvertURLStringToNormal(theMap.theValues[i], false);
    result = HtmlRoutines::ChopCGIString(currentQuestion, currentQuestionMap, commentsOnFailure);
    if (!result) {
      commentsOnFailure << "Failed to interpret as key-value pair: "
      << currentQuestion << ". ";
      continue;
    }
    if (currentQuestionMap.Contains("numCorrectSubmissions")) {
      currentA.numCorrectSubmissions =
      atoi(currentQuestionMap.GetValueCreate("numCorrectSubmissions").c_str());
    }
    if (currentQuestionMap.Contains("numSubmissions")) {
      currentA.numSubmissions =
      atoi(currentQuestionMap.GetValueCreate("numSubmissions").c_str());
    }
    if (currentQuestionMap.Contains("firstCorrectAnswer")) {
      currentA.firstCorrectAnswerURLed = currentQuestionMap.GetValueCreate("firstCorrectAnswer");
      currentA.firstCorrectAnswerClean = HtmlRoutines::ConvertURLStringToNormal(currentA.firstCorrectAnswerURLed, false);
      currentA.firstCorrectAnswerURLed = HtmlRoutines::ConvertStringToURLString(currentA.firstCorrectAnswerClean, false); //url-encoding back the cleaned up answer:
      //this protects from the possibility that currentA.firstCorrectAnswerURLed was not encoded properly.
    }
  }
//  this->CheckConsistency();
  return result;
}

bool ProblemData::LoadFromJSON(const JSData& inputData, std::stringstream& commentsOnFailure) {
  MacroRegisterFunctionWithName("ProblemData::LoadFromJSON");
  (void) commentsOnFailure;
  this->Points = 0;
  this->numCorrectlyAnswered = 0;
  this->totalNumSubmissions = 0;
  this->flagRandomSeedGiven = false;
  if (global.UserRequestRequiresLoadingRealExamData()) {
    if (inputData.objects.Contains("randomSeed")) {
      this->randomSeed = static_cast<unsigned>(atoi(
        inputData.objects.GetValueConstCrashIfNotPresent("randomSeed").theString.c_str()
      ));
      this->flagRandomSeedGiven = true;
    }
  }
  this->theAnswers.Clear();
  bool result = true;
  for (int i = 0; i < inputData.objects.size(); i ++) {
    if (inputData.objects.theKeys[i] == "randomSeed") {
      continue;
    }
    this->AddEmptyAnswerIdOnTop(HtmlRoutines::ConvertURLStringToNormal(inputData.objects.theKeys[i], false));
    Answer& currentA = *this->theAnswers.theValues.LastObject();
    JSData currentQuestionJSON = inputData.objects.theValues[i];
    if (currentQuestionJSON.objects.Contains("numCorrectSubmissions")) {
      currentA.numCorrectSubmissions =
      atoi(currentQuestionJSON.objects.GetValueConstCrashIfNotPresent("numCorrectSubmissions").theString.c_str());
    }
    if (currentQuestionJSON.objects.Contains("numSubmissions")) {
      currentA.numSubmissions =
      atoi(currentQuestionJSON.objects.GetValueConstCrashIfNotPresent("numSubmissions").theString.c_str());
    }
    if (currentQuestionJSON.objects.Contains("firstCorrectAnswer")) {
      currentA.firstCorrectAnswerURLed = currentQuestionJSON.objects.GetValueConstCrashIfNotPresent("firstCorrectAnswer").theString;
      currentA.firstCorrectAnswerClean = HtmlRoutines::ConvertURLStringToNormal(currentA.firstCorrectAnswerURLed, false);
      currentA.firstCorrectAnswerURLed = HtmlRoutines::ConvertStringToURLString(currentA.firstCorrectAnswerClean, false); //url-encoding back the cleaned up answer:
      //this protects from the possibility that currentA.firstCorrectAnswerURLed was not encoded properly,
      //say, by an older version of the calculator
    }
  }
//  this->CheckConsistency();
  return result;
}

bool UserCalculator::InterpretDatabaseProblemDatA(const std::string& theInfo, std::stringstream& commentsOnFailure) {
  MacroRegisterFunctionWithName("UserCalculator::InterpretDatabaseProblemData");
  MapList<std::string, std::string, MathRoutines::HashString> theMap;
  if (!HtmlRoutines::ChopCGIString(theInfo, theMap, commentsOnFailure)) {
    return false;
  }
  this->theProblemData.Clear();
  this->theProblemData.SetExpectedSize(theMap.size());
  bool result = true;
  ProblemData reader;
  std::string probNameNoWhiteSpace;
  for (int i = 0; i < theMap.size(); i ++) {
    if (!reader.LoadFroM(HtmlRoutines::ConvertURLStringToNormal(theMap.theValues[i], false), commentsOnFailure)) {
      result = false;
      continue;
    }
    probNameNoWhiteSpace = StringRoutines::StringTrimWhiteSpace(HtmlRoutines::ConvertURLStringToNormal(theMap.theKeys[i], false));
    if (probNameNoWhiteSpace == "") {
      continue;
    }
    this->theProblemData.SetKeyValue(probNameNoWhiteSpace, reader);
  }
  return result;
}

bool UserCalculator::InterpretDatabaseProblemDataJSON(const JSData& theData, std::stringstream& commentsOnFailure) {
  MacroRegisterFunctionWithName("UserCalculator::InterpretDatabaseProblemDataJSON");
  this->theProblemData.Clear();
  this->theProblemData.SetExpectedSize(theData.objects.size());
  bool result = true;
  ProblemData reader;
  std::string probNameNoWhiteSpace;
  for (int i = 0; i < theData.objects.size(); i ++) {
    if (!reader.LoadFromJSON(theData.objects.theValues[i], commentsOnFailure)) {
      result = false;
      continue;
    }
    probNameNoWhiteSpace = StringRoutines::StringTrimWhiteSpace(
      HtmlRoutines::ConvertURLStringToNormal(theData.objects.theKeys[i], false
    ));
    if (probNameNoWhiteSpace == "") {
      continue;
    }
    this->theProblemData.SetKeyValue(probNameNoWhiteSpace, reader);
  }
  return result;
}

bool UserCalculator::Iexist(std::stringstream* comments) {
  MacroRegisterFunctionWithName("UserCalculator::Iexist");
  JSData notUsed;
  return Database::get().FindOneFromSome(
    this->GetFindMeFromUserNameQuery(), notUsed, comments
  );
}

bool UserCalculator::ComputeAndStoreActivationToken(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("UserCalculator::ComputeAndStoreActivationToken");
  TimeWrapper now;
  now.AssignLocalTime();
  // activation token randomness strength: rand() has some randomness in it
  // and so does the timer, so the add up (actually, multiply).
  // This is not a very strong protection for the time being, but as activation tokens are supposed to be
  // one-time use this should present no practical danger.
  List<unsigned char> activationToken;

  Crypto::GetRandomBytesSecureInternalMayLeaveTracesInMemory(activationToken, 16);
  this->actualActivationToken = Crypto::ConvertListUnsignedCharsToBase64(activationToken, true);
  QueryExact findUserQuery(DatabaseStrings::tableUsers, DatabaseStrings::labelUsername, this->username);
  JSData updateUser;
  updateUser[DatabaseStrings::labelActivationToken] = this->actualActivationToken;
  if (!Database::get().UpdateOne(
    findUserQuery, updateUser, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Setting activation token failed.";
    }
    this->actualActivationToken = "";
    return false;
  }
  return true;
}

bool UserCalculator::ComputeAndStoreActivationEmailAndTokens(
  std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("UserCalculator::ComputeAndStoreActivationEmailAndTokens");
  if (!this->ComputeAndStoreActivationToken(commentsOnFailure)) {
    return false;
  }
  return this->ComputeAndStoreActivationStats(commentsOnFailure, commentsGeneral);
}

bool UserCalculator::ComputeAndStoreActivationStats(
  std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
) {
  if (!global.flagDatabaseCompiled) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Compiled without database support. ";
    }
    return false;
  }
  MacroRegisterFunctionWithName("UserCalculator::ComputeAndStoreActivationStats");
  std::string activationAddress = this->GetActivationAddressFromActivationToken(
    this->actualActivationToken, global.hostWithPort, this->username, this->email
  );
  JSData emailStatQuery;
  QueryExact findEmail(DatabaseStrings::tableEmailInfo, DatabaseStrings::labelEmail, this->email);
  Database::get().FindOne(
    findEmail, emailStatQuery, commentsOnFailure
  );
  std::string lastEmailTime, emailCountForThisEmail;
  lastEmailTime = emailStatQuery[DatabaseStrings::labelLastActivationEmailTime].theString;
  emailCountForThisEmail = emailStatQuery[DatabaseStrings::labelNumActivationEmails].theString;

  LargeInteger numActivationsThisEmail = 0;
  if (emailCountForThisEmail != "") {
    numActivationsThisEmail.AssignString(emailCountForThisEmail);
  }
  numActivationsThisEmail ++;
  TimeWrapper now, lastActivationOnThisEmail, lastActivationOnThisAccount;
  now.AssignLocalTime();
  if (lastEmailTime != "") {
    lastActivationOnThisEmail.operator=(lastEmailTime);
    lastActivationOnThisAccount.operator=(this->timeOfActivationTokenCreation);
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "<br>Last activation on this email, GM time: "
      << lastActivationOnThisEmail.ToStringGM() << ".\n"
      << "<br>Last activation on this account, GM time: "
      << lastActivationOnThisEmail.ToStringGM() << ".\n";
    }
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral
    << "<br>Total activations (attempted on) this email: "
    << numActivationsThisEmail.ToString() << ".\n<br>\n";
  }
  QueryExact findQueryInUsers(DatabaseStrings::tableUsers, "", "");
  JSData updateUser;
  updateUser[DatabaseStrings::labelTimeOfActivationTokenCreation] = now.ToString();
  if (this->userId != "") {
    findQueryInUsers.SetLabelValue(DatabaseStrings::labelUserId, this->userId);
  } else if (this->username != "") {
    findQueryInUsers.SetLabelValue(DatabaseStrings::labelUsername, this->username);
  } else {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "This shouldn't happen: both the username and the user id are empty. ";
    }
    return false;
  }
  if (!Database::get().UpdateOne(
    findQueryInUsers, updateUser, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to set activationTokenCreationTime. ";
    }
    return false;
  }
  emailStatQuery[DatabaseStrings::labelLastActivationEmailTime] = now.ToString();
  emailStatQuery[DatabaseStrings::labelNumActivationEmails] = numActivationsThisEmail.ToString();
  emailStatQuery[DatabaseStrings::labelActivationToken] = this->actualActivationToken;
  emailStatQuery[DatabaseStrings::labelUsernameAssociatedWithToken] = this->username;
  if (!Database::get().UpdateOne(
    findEmail, emailStatQuery, commentsOnFailure
  )) {
    return false;
  }
  this->activationEmailSubject = "NO REPLY: Activation of your math account. ";
  std::stringstream emailBody;
  emailBody << "Dear user,"
  << "\n\nto confirm your email change at our website "
  << global.hostWithPort
  << ", please follow the activation link below.\n\n "
  << activationAddress
  << "\n\nSincerely, \nthe calculator-algebra.org team";
  this->activationEmail = emailBody.str();
  return true;
}

List<QueryExact> UserCalculatorData::GetFindMeFromUserNameQuery() {
  List<QueryExact> result;
  if (this->username != "") {
    QueryExact findByUsername(DatabaseStrings::tableUsers, DatabaseStrings::labelUsername, this->username);
    result.AddOnTop(findByUsername);
  }
  if (this->email != "") {
    QueryExact findByEmail(DatabaseStrings::tableUsers, DatabaseStrings::labelEmail, this->email);
    result.AddOnTop(findByEmail);
  }
  if (result.size == 0) {
    global.fatal << "User with find query not allowed to have neither username nor email. " << global.fatal;
  }
  return result;
}

bool UserCalculator::StoreProblemDataToDatabasE(std::stringstream& commentsOnFailure) {
  MacroRegisterFunctionWithName("UserCalculator::StoreProblemDataToDatabasE");
  std::stringstream problemDataStream;
  for (int i = 0; i < this->theProblemData.size(); i ++) {
    problemDataStream << HtmlRoutines::ConvertStringToURLString(this->theProblemData.theKeys[i], false) << "="
    << HtmlRoutines::ConvertStringToURLString(this->theProblemData.theValues[i].StorE(), false) << "&";
  }
  JSData setQuery;
  setQuery[DatabaseStrings::labelProblemDatA] = problemDataStream.str();
  return Database::get().UpdateOneFromSome(
    this->GetFindMeFromUserNameQuery(), setQuery, &commentsOnFailure
  );
}

bool UserCalculator::StoreProblemDataToDatabaseJSON(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("UserCalculator::StoreProblemDataToDatabaseJSON");
  JSData problemData;
  for (int i = 0; i < this->theProblemData.size(); i ++) {
    JSData nextProblem = this->theProblemData.theValues[i].StoreJSON();
    if (nextProblem.objects.size() == 0) {
      continue;
    }
    problemData[HtmlRoutines::ConvertStringToURLString(this->theProblemData.theKeys[i], false)] = nextProblem;
  }
  global.userDefault.problemDataJSON = problemData;
  JSData updateData;
  updateData[DatabaseStrings::labelProblemDataJSON] = problemData;
  return Database::get().UpdateOneFromSome(
    this->GetFindMeFromUserNameQuery(), updateData, commentsOnFailure
  );
}

bool Database::User::AddUsersFromEmails(
  const std::string& emailList,
  const std::string& userPasswords,
  std::string& userRole,
  std::string& userGroup,
  std::stringstream& comments,
  int& outputNumNewUsers,
  int& outputNumUpdatedUsers
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::AddUsersFromEmails");
  global.millisecondsMaxComputation = 100000; //100 seconds
  global.millisecondsReplyAfterComputation = 200000; // 200 seconds
  List<std::string> theEmails, thePasswords;
  StringRoutines::StringSplitDefaultDelimiters(emailList, theEmails);
  StringRoutines::StringSplitDefaultDelimiters(userPasswords, thePasswords);
  if (thePasswords.size > 0) {
    if (thePasswords.size != theEmails.size) {
      comments << "Different number of usernames/emails and passwords: " << theEmails.size << " emails and "
      << thePasswords.size << " passwords. "
      << "If you want to enter usernames without password, you need to leave the password input box empty. ";
      return false;
    }
  }
  UserCalculator currentUser;
  bool result = true;
  bool doSendEmails = false;
  outputNumNewUsers = 0;
  outputNumUpdatedUsers = 0;
  for (int i = 0; i < theEmails.size; i ++) {
    currentUser.reset();
    currentUser.username = theEmails[i];
    currentUser.courseInDB = global.GetWebInput("courseHome");
    currentUser.sectionInDB = userGroup;
    currentUser.instructorInDB = global.userDefault.username;
    currentUser.email = theEmails[i];
    currentUser.userRole = userRole;

    bool isEmail = true;
    if (theEmails[i].find('@') == std::string::npos) {
      isEmail = false;
      currentUser.email = "";
    } else {
      currentUser.email = theEmails[i];
    }
    JSData currentUserData;
    List<QueryExact> findUser;
    findUser.AddOnTop(QueryExact(DatabaseStrings::tableUsers, DatabaseStrings::labelUsername, currentUser.username));
    findUser.AddOnTop(QueryExact(DatabaseStrings::tableUsers, DatabaseStrings::labelEmail, currentUser.email));
    if (!this->owner->FindOneFromSome(
      findUser, currentUserData, &comments
    )) {
      if (!currentUser.Iexist(&comments)) {
        if (!currentUser.StoreToDB(false, &comments)) {
          comments << "Failed to create user: " << currentUser.username;
          result = false;
          continue;
        } else {
          outputNumNewUsers ++;
        }
      }
      if (isEmail) {
        this->owner->UpdateOneFromSome(findUser, currentUserData, &comments);
      }
    } else {
      outputNumUpdatedUsers ++;
      //currentUser may have its updated entries modified by the functions above.
      if (!this->owner->UpdateOneFromSome(
        findUser, currentUser.ToJSON(), &comments
      )) {
        result = false;
      }
    }
    if (thePasswords.size == 0 || thePasswords.size != theEmails.size) {
      if (currentUser.actualHashedSaltedPassword == "" && currentUser.actualAuthenticationToken == "") {
        if (!currentUser.ComputeAndStoreActivationToken(&comments)) {
          result = false;
        }
      }
    } else {
      currentUser.enteredPassword = HtmlRoutines::ConvertStringToURLString(thePasswords[i], false);
      //<-Passwords are ONE-LAYER url-encoded
      //<-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
      //<-Incoming pluses must be re-coded as spaces (%20).
      if (!currentUser.SetPassword(&comments)) {
        result = false;
      }
      JSData activatedJSON;
      activatedJSON[DatabaseStrings::labelActivationToken] = "activated";
      this->owner->UpdateOneFromSome(findUser, activatedJSON, &comments);
      if (currentUser.email != "") {
        currentUser.ComputeAndStoreActivationStats(&comments, &comments);
      }
    }
  }
  if (!result) {
    comments << "<br>Failed to create all users. ";
  }
  if (doSendEmails) {
    std::stringstream* commentsGeneralSensitive = nullptr;
    if (global.UserDefaultHasAdminRights()) {
      commentsGeneralSensitive = &comments;
    }
    if (!Database::User::SendActivationEmail(theEmails, &comments, &comments, commentsGeneralSensitive)) {
      result = false;
    }
  }
  return result;
}

bool UserCalculator::GetActivationAbsoluteAddress(std::string& output, std::stringstream& comments) {
  MacroRegisterFunctionWithName("UserCalculator::GetActivationAbsoluteAddress");
  return this->GetActivationAddress(
    output, global.hopefullyPermanent_HTTPS_WebAdressOfServerExecutable, comments
  );
}

bool UserCalculator::GetActivationAddress(
  std::string& output, const std::string& calculatorBase, std::stringstream& comments
) {
  MacroRegisterFunctionWithName("UserCalculator::GetActivationAddress");
  if (!this->LoadFromDB(&comments, &comments)) {
    return false;
  }
  this->actualActivationToken = this->GetSelectedRowEntry("activationToken");
  if (this->actualActivationToken == "") {
    comments << "Failed to fetch activation token for user: " << this->username;
    return false;
  }
  if (this->actualActivationToken == "activated") {
    comments << "Account of user: " << this->username << "already activated";
    return false;
  }
  output = this->GetActivationAddressFromActivationToken(
    this->actualActivationToken, calculatorBase, this->username, this->email
  );
  return true;
}

bool CalculatorDatabaseFunctions::innerRepairDatabaseEmailRecords(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("DatabaseRoutines::innerRepairDatabaseEmailRecords");
  std::stringstream out;
  if (!global.UserDefaultHasAdminRights()) {
    out << "Function available to logged-in admins only. ";
    return output.AssignValue(out.str(), theCommands);
  }
  global.millisecondsMaxComputation = 20000000; //20k seconds, ok as this is admin-only.
  (void) input;//prevent unused parameter, portable
  out << "Testing/repairing database ... Comments:<br>";
  out << "NOT IMPLEMENTED YET";
  return output.AssignValue(out.str(), theCommands);
/*

  std::stringstream comments;
  if (!theRoutines.ColumnExists(DatabaseStrings::labelCourseInfo, DatabaseStrings::tableUsers, out)) {
    out << "Column " << DatabaseStrings::labelCourseInfo << ": "
    << " does not exist, creating ...";
    if (!theRoutines.CreateColumn(DatabaseStrings::labelCourseInfo, DatabaseStrings::tableUsers, out))
      out << "Failed to create column: " << DatabaseStrings::labelCourseInfo << "<br>";
  }
  out << comments.str();
  List<List<std::string> > theUserTable;
  List<std::string> labels;
  if (!theRoutines.FetchAllUsers(theUserTable, labels, out))
    return output.AssignValue(out.str(), theCommands);
  int emailColumn = labels.GetIndex("email");
  int usernameColumn = labels.GetIndex(DatabaseStrings::labelUsername);
  int passwordColumn = labels.GetIndex("password");
  int activationTokenColumn = labels.GetIndex("activationToken");
  ProgressReport theReport;
  if (emailColumn == - 1) {
    out << "Couldn't find email column. ";
    return output.AssignValue(out.str(), theCommands);
  }
  if (usernameColumn == - 1) {
    out << "Couldn't find username column. ";
    return output.AssignValue(out.str(), theCommands);
  }
  if (passwordColumn == - 1) {
    out << "Couldn't find password column. ";
    return output.AssignValue(out.str(), theCommands);
  }
  if (activationTokenColumn == - 1) {
    out << "Couldn't find activation token column. ";
    return output.AssignValue(out.str(), theCommands);
  }
  HashedList<std::string, MathRoutines::hashString> emailsRegistered;
  for (int i = 0; i < theUserTable.size; i ++) {
    std::string currentEmail = theUserTable[i][emailColumn];
    if (emailsRegistered.Contains(currentEmail)) {
      out << "Fatal error: email " << currentEmail << "repeated. ";
      return output.AssignValue(out.str(), theCommands);
    }
  }
  int numCorrections = 0;
  UserCalculator currentUser;
  currentUser.currentTable = DatabaseStrings::tableUsers;
  for (int i = 0; i < theUserTable.size; i ++) {
    std::string currentUserName = theUserTable[i][usernameColumn];
    std::string currentEmail = theUserTable[i][emailColumn];
    if (currentEmail != "")
      continue;
    if (!EmailRoutines::IsOKEmail(currentUserName, 0))
      continue;
    if (emailsRegistered.Contains(currentEmail)) {
      out << "<br>This shouldn't happen: username: " << currentUserName
      << " has no corresponding email. At the same time, the username appears to be an email, and "
      << " that same email has been registered with another account. "
      << " Please resolve the matter with the user. ";
      continue;
    }
    currentUser.username = currentUserName;
    currentUser.email = currentUserName;
    if (!currentUser.SetColumnEntry("email", currentUser.email.value, theRoutines, &comments)) {
      out << "<br>This shouldn't happen: failed to set email for " << currentUser.username.value << ". ";
      continue;
    }
    numCorrections ++;
    std::stringstream currentCorrection;
    currentCorrection << "<br>Correction " << numCorrections << ": user " << currentUser.username.value
    << " had no email but the username appears to be a valid email. "
    << "I have therefore set the user's email address equal to the user's username. ";
    out << currentCorrection.str();
    theReport.Report(currentCorrection.str());
  }
  for (int i = 0; i < theUserTable.size; i ++) {
    currentUser.username = theUserTable[i][usernameColumn];
    std::string currentPassword = theUserTable[i][passwordColumn];
    std::string currentActivationToken = theUserTable[i][activationTokenColumn];
    if (currentActivationToken == "activated")
      continue;
    if (currentPassword == "")
      continue;
    numCorrections ++;
    std::stringstream currentCorrection;
    currentCorrection << "<br>Correction " << numCorrections << ": "
    << " user: " << currentUser.username.value << " has a password but his activation token "
    << "has not been set to activated. Fixing. ";
    currentUser.SetColumnEntry("activationToken", "activated", theRoutines, &comments);
    out << currentCorrection.str();
    theReport.Report(currentCorrection.str());
  }
  for (int i = 0; i < theUserTable.size; i ++) {
    currentUser.reset();
    std::stringstream currentUserComments;
    currentUser.username = theUserTable[i][usernameColumn];
    currentUser.FetchOneUserRow(theRoutines, &out, &currentUserComments);
    if (currentUserComments.str().size() > 0) {
      numCorrections ++;
      std::stringstream currentCorrection;
      currentCorrection
      << "<br>Correction " << numCorrections << ": "
      << " user: " << currentUser.username.value << " has old course info format in the database. "
      << currentUserComments.str();
      out << currentCorrection.str();
      theReport.Report(currentCorrection.str());
    }
  }
  return output.AssignValue(out.str(), theCommands);*/
}

bool EmailRoutines::SendEmailWithMailGun(
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsGeneralSensitive
) {
  MacroRegisterFunctionWithName("EmailRoutines::SendEmailWithMailGun");
  std::string mailGunKey, hostnameToSendEmailFrom;
  if (!FileOperations::LoadFileToStringVirtual_AccessUltraSensitiveFoldersIfNeeded(
    "certificates/mailgun-api.txt", mailGunKey, true, true, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Could not find mailgun key. The key must be located in file: "
      << "<br>\ncertificates/mailgun-api.txt\n<br>\n "
      << "The file must be uploaded manually to the server. ";
    }
    return false;
  }
  if (!FileOperations::LoadFileToStringVirtual_AccessUltraSensitiveFoldersIfNeeded(
    "certificates/mailgun-hostname.txt", hostnameToSendEmailFrom, true, true, nullptr
  )) {
    hostnameToSendEmailFrom = global.hostNoPort;
    if (global.UserDefaultHasAdminRights() && commentsGeneral != nullptr) {
      *commentsGeneral << "Did not find the mailgun hostname file: certificates/mailgun-hostname.txt. Using the "
      << "domain name: " << hostnameToSendEmailFrom << " instead. ";
    }
  } else {
    hostnameToSendEmailFrom = StringRoutines::StringTrimWhiteSpace(hostnameToSendEmailFrom);
    if (global.UserDefaultHasAdminRights() && commentsGeneral != nullptr) {
      *commentsGeneral << "Hostname loaded: "
      << HtmlRoutines::ConvertStringToURLString(hostnameToSendEmailFrom, false);
    }
  }
  if (mailGunKey.size() > 0) {
    mailGunKey.resize(mailGunKey.size() - 1);
  }
  global << "Sending email via "
  << "https://api.mailgun.net/v3/mail2."
  << hostnameToSendEmailFrom
  << "/messages " << logger::endL;
  std::stringstream commandToExecute;
  commandToExecute << "curl -s --user 'api:" << mailGunKey
  << "' ";
  commandToExecute
  << "https://api.mailgun.net/v3/mail2."
  << hostnameToSendEmailFrom
  << "/messages ";
  commandToExecute << "-F from='Automated Email "
  << "<noreply@mail2."
  << hostnameToSendEmailFrom
  << ">' ";
  if (this->toEmail == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Receiver address is missing. ";
    }
    return false;
  }
  if (this->subject == "" && this->emailContent == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty emails not allowed. ";
    }
    return false;
  }
  //WARNING: calling command line
  //that includes user inputs.
  //Those MUST be sanitized (else an attacker would use username with " characters
  //to execute arbitrary code.
  commandToExecute << "-F to=\""
  << HtmlRoutines::ConvertStringEscapeQuotesAndBackslashes(this->toEmail) << "\" "
  << "-F subject=\""
  << HtmlRoutines::ConvertStringEscapeQuotesAndBackslashes(this->subject) << "\" "
  << "-F text=\""
  << HtmlRoutines::ConvertStringEscapeQuotesAndBackslashes(this->emailContent)
  << "\"";
  std::string commandResult = global.CallSystemWithOutput(commandToExecute.str());
  if (commentsGeneralSensitive != nullptr) {
    *commentsGeneralSensitive << "Command: " << HtmlRoutines::ConvertStringToHtmlString(commandToExecute.str(), true);
    bool isBad = false;
    if (commandResult.find("Forbidden") != std::string::npos) {
      isBad = true;
    }
    *commentsGeneralSensitive << "<br>Result:<br>";
    if (isBad) {
      *commentsGeneralSensitive << "<b style ='color:red'>";
    }
    *commentsGeneralSensitive << HtmlRoutines::ConvertStringToHtmlString(commandResult, true);
    if (isBad) {
      *commentsGeneralSensitive << "</b>";
    }
  }
  return true;
}

List<bool> EmailRoutines::recognizedEmailCharacters;
List<bool>& EmailRoutines::GetRecognizedEmailChars() {
  if (recognizedEmailCharacters.size == 0) {
    recognizedEmailCharacters.initializeFillInObject(256, false);
    std::string theChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    theChars += "0123456789";
    theChars += "@";
    theChars += "!#$%&'*+-/=?.";
    for (unsigned i = 0; i < theChars.size(); i ++) {
      recognizedEmailCharacters[static_cast<unsigned char>(theChars[i])] = true;
    }
  }
  return EmailRoutines::recognizedEmailCharacters;
}

bool EmailRoutines::IsOKEmail(const std::string& input, std::stringstream* commentsOnError) {
  MacroRegisterFunctionWithName("EmailRoutines::IsOKEmail");
  if (input.size() == 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Empty email not allowed. ";
    }
    return false;
  }
  int numAts = 0;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (!EmailRoutines::GetRecognizedEmailChars()[static_cast<unsigned char>(input[i])]) {
      if (commentsOnError != nullptr) {
        *commentsOnError << "Email: " << input << " contains the unrecognized character "
        << input[i] << ". ";
      }
      return false;
    }
    if (input[i] == '@') {
      numAts ++;
    }
  }
  if (numAts != 1 && commentsOnError != nullptr) {
    *commentsOnError << "Email: "
    << input << " is required to have exactly one @ character. ";
  }
  return numAts == 1;
}

EmailRoutines::EmailRoutines() {
  this->fromEmail = "calculator.todor.milev@gmail.com";
  //this->ccEmail ="todor.milev@gmail.com";
  this->smtpWithPort = "smtp.gmail.com:587";
  this->fromEmailAuth = Crypto::ConvertStringToBase64URL("A good day to use a computer algebra system");
}

bool Database::User::LoginViaGoogleTokenCreateNewAccountIfNeeded(
  UserCalculatorData& theUseR,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral,
  bool& tokenIsGood
) {
  (void) commentsOnFailure;
  (void) theUseR;
  (void) commentsGeneral;
  if (!global.flagDatabaseCompiled) {
    return Database::User::LoginNoDatabaseSupport(theUseR, commentsGeneral);
  }
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::LoginViaGoogleTokenCreateNewAccountIfNeeded");
  UserCalculator userWrapper;
  userWrapper.::UserCalculatorData::operator=(theUseR);
  if (userWrapper.enteredGoogleToken == "") {
    return false;
  }
  if (!Crypto::VerifyJWTagainstKnownKeys(userWrapper.enteredGoogleToken, commentsOnFailure, commentsGeneral)) {
    return false;
  }
  tokenIsGood = true;
  JSONWebToken theToken;
  if (!theToken.AssignString(userWrapper.enteredGoogleToken, commentsOnFailure)) {
    return false;
  }
  JSData theData;
  if (!theData.readstring(theToken.claimsJSON, commentsOnFailure)) {
    return false;
  }
  if (theData.GetValue("email").theType != JSData::token::tokenString) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Could not find email entry in the json data " << theData.ToString(true);
    }
    return false;
  }
  userWrapper.email = theData.GetValue("email").theString;
  userWrapper.username = "";
  if (!userWrapper.Iexist(commentsOnFailure)) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "User with email " << userWrapper.email << " does not exist. ";
    }
    userWrapper.username = userWrapper.email;
    if (!userWrapper.StoreToDB(false, commentsOnFailure)) {
      return false;
    }
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "Created new user with username: " << userWrapper.username;
    }
    theUseR = userWrapper;
    return true;
  }
  if (!userWrapper.LoadFromDB(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to fetch user with email " << userWrapper.email << ". ";
    }
    return false;
  }
  if (userWrapper.actualAuthenticationToken == "") {
    userWrapper.ResetAuthenticationToken(commentsOnFailure);
  }
  theUseR = userWrapper;
  return true;
}

bool Database::User::LoginNoDatabaseSupport(
  UserCalculatorData& theUser, std::stringstream* commentsGeneral
) {
  if (global.flagDatabaseCompiled) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "Database support is available, yet you requested no-database login. "
      << "The command 'make -j10 noMongo=1' compiles the calculator without database support.";
    }
    return false;
  }
  if (!global.flagDisableDatabaseLogEveryoneAsAdmin) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "Logging-in without database is not allowed. "
      << "To allow, set "
      << Configuration::disableDatabaseLogEveryoneAsAdmin
      << " to true in the server's configuration.json file. ";
    }
    return false;
  }
  // When the database is disabled, we assume the user is an admin.
  // The purpose of this is that
  // users who do not have a mongoDB installed
  // (or have troubles accessing it for some reason)
  // can still use the admin functions of the calculator, for example,
  // modify problem files from the one-page app.
  theUser.userRole = UserCalculatorData::Roles::admin;
  theUser.actualAuthenticationToken = "compiledWithoutDatabaseSupport";
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "Automatic login as admin: calculator compiled without DB. ";
  }
  return true;
}

bool Database::User::LoginViaDatabase(
  UserCalculatorData& theUseR,
  std::stringstream* commentsOnFailure
) {
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    return Database::User::LoginNoDatabaseSupport(theUseR, commentsOnFailure);
  }
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::LoginViaDatabase");
  UserCalculator userWrapper;
  userWrapper.::UserCalculatorData::operator=(theUseR);
  if (userWrapper.Authenticate(commentsOnFailure)) {
    theUseR = userWrapper;
    return true;
  }
  if (
    userWrapper.enteredAuthenticationToken != "" &&
    userWrapper.enteredAuthenticationToken != "0" &&
    commentsOnFailure != nullptr
  ) {
    *commentsOnFailure << "<b> Authentication of user: " << userWrapper.username
    << " with token " << userWrapper.enteredAuthenticationToken << " failed. </b>";
  }
  if (userWrapper.enteredActivationToken != "") {
    if (
      global.userCalculatorRequestType == WebAPI::request::changePassword ||
      global.userCalculatorRequestType == "changePasswordPage" ||
      global.userCalculatorRequestType == WebAPI::request::activateAccountJSON
    ) {
      if (
        userWrapper.actualActivationToken != "activated" &&
        userWrapper.actualActivationToken != "" &&
        userWrapper.actualActivationToken != "error"
      ) {
        if (userWrapper.enteredActivationToken == userWrapper.actualActivationToken) {
          theUseR = userWrapper;
          return true;
        }
      } else if (commentsOnFailure != nullptr) {
        if (userWrapper.actualActivationToken != "error") {
          *commentsOnFailure << "<b>Account already activated. </b>";
        } else {
          *commentsOnFailure << "<b>An error during activation ocurred.</b>";
        }
      }
    } else {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Activation token entered but the user request type: "
        << global.userCalculatorRequestType
        << " does not allow login with activation token. ";
      }
    }
  }
  if (userWrapper.username == "admin" && userWrapper.enteredPassword != "") {
    if (!userWrapper.Iexist(nullptr)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "<b>First login of user admin: setting admin password.</b> ";
      }
      global << logger::yellow << "First login of user admin: setting admin password." << logger::endL;
      userWrapper.actualActivationToken = "activated";
      userWrapper.userRole = "admin";
      if (!userWrapper.StoreToDB(true, commentsOnFailure)) {
        global << logger::red << "Failed to store admin pass to database. ";
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Failed to store admin pass to database. ";
          global << commentsOnFailure->str();
        }
      }
      theUseR = userWrapper;
      return true;
    }
  }
  return false;
}

void UserCalculator::ComputeHashedSaltedPassword() {
  MacroRegisterFunctionWithName("UserCalculator::ComputeShaonedSaltedPassword");
  this->usernameHashedPlusPassWordHashed.resize(Crypto::LengthSha3DefaultInBytes * 2);
  List<unsigned char> hasher;
  Crypto::computeSha3_256(this->enteredPassword, hasher);
  //<-careful copying entered password around. We want to avoid leaving
  //passwords in non-zeroed memory, even if properly freed (to the extent possible and practical).
  for (unsigned i = 0; i < Crypto::LengthSha3DefaultInBytes; i ++) {
    this->usernameHashedPlusPassWordHashed[i + Crypto::LengthSha3DefaultInBytes] = static_cast<char>(hasher[i]);
  }
  Crypto::computeSha3_256(this->username, hasher);
  for (unsigned i = 0; i < Crypto::LengthSha3DefaultInBytes; i ++) {
    this->usernameHashedPlusPassWordHashed[i] = static_cast<char>(hasher[i]);
  }
  this->enteredHashedSaltedPassword = Crypto::computeSha3_256OutputBase64URL(this->usernameHashedPlusPassWordHashed);
}

bool UserCalculator::StoreToDB(bool doSetPassword, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("UserCalculator::StoreToDB");
  QueryExact findUser(DatabaseStrings::tableUsers, DatabaseStrings::labelUsername, this->username);
  if (this->enteredPassword != "" && doSetPassword) {
    this->ComputeHashedSaltedPassword();
    this->actualHashedSaltedPassword = this->enteredHashedSaltedPassword;
  }
  JSData setUser = this->ToJSON();

  return Database::get().UpdateOne(
    findUser, setUser, commentsOnFailure
  );
}

//meld comment
#include "crypto.h"

std::string UserCalculator::GetActivationAddressFromActivationToken(
  const std::string& theActivationToken,
  const std::string& calculatorBase,
  const std::string& inputUserNameUnsafe,
  const std::string& inputEmailUnsafe
) {
  MacroRegisterFunctionWithName("UserCalculator::GetActivationLinkFromActivationToken");
  std::stringstream out;
  if (StringRoutines::StringBeginsWith(calculatorBase, "localhost")) {
    out << "https://" << calculatorBase;
  } else if (StringRoutines::StringBeginsWith(calculatorBase, "https://localhost")) {
    out << calculatorBase;
  } else {
    out << global.hopefullyPermanentWebAdress;
  }
  JSData theJS;
  theJS[DatabaseStrings::labelActivationToken] = theActivationToken;
  theJS[DatabaseStrings::labelUsername] = inputUserNameUnsafe;
  theJS[DatabaseStrings::labelCalculatorRequest] = WebAPI::request::activateAccountJSON;
  theJS[DatabaseStrings::labelEmail] = inputEmailUnsafe;
  theJS[DatabaseStrings::labelCurrentPage] = DatabaseStrings::labelPageActivateAccount;
  out << global.DisplayNameExecutableApp
  << "#" << HtmlRoutines::ConvertStringToURLString(theJS.ToString(false), false);
  return out.str();
}
