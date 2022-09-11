#include "database.h"
#include "crypto.h"
#include "webserver.h"
#include "system_functions_global_objects.h"
#include "calculator_database_mongo.h"
#include "calculator_problem_storage.h"
#include "string_constants.h"

std::string EmailRoutines::webAdress = "";
std::string EmailRoutines::sendEmailFrom = "";
bool Database::User::setPassword(
  const std::string& inputUsername,
  const std::string& inputNewPassword,
  std::string& outputAuthenticationToken,
  std::stringstream& comments
) {
  STACK_TRACE("Database::User::setPassword");
  if (!global.flagLoggedIn) {
    comments << "Changing passwords allowed for logged-in users only. ";
    return false;
  }
  UserCalculator user;
  user.username = inputUsername;
  user.enteredPassword = inputNewPassword;
  bool result = user.setPassword(&comments);
  user.resetAuthenticationToken(&comments);
  outputAuthenticationToken = user.actualAuthenticationToken;
  return result;
}

bool Database::User::userExists(
  const std::string& inputUsername, std::stringstream& comments
) {
  STACK_TRACE("Database::User::userExists");
  if (!global.flagLoggedIn) {
    return false;
  }
  JSData userQuery;
  userQuery[DatabaseStrings::labelUsername] = inputUsername;
  List<JSData> allUsers;
  this->owner->findFromJSON(
    DatabaseStrings::tableUsers,
    userQuery,
    allUsers,
    - 1,
    nullptr,
    &comments
  );
  return allUsers.size > 0;
}

bool Database::User::userDefaultHasInstructorRights() {
  STACK_TRACE("Database::User::userDefaultHasInstructorRights");
  if (global.userDefaultHasAdminRights()) {
    return true;
  }
  if (!global.flagDatabaseCompiled) {
    return true;
  }
  if (!global.flagLoggedIn) {
    return false;
  }
  return
  global.userDefault.userRole == UserCalculator::Roles::administator ||
  global.userDefault.userRole == UserCalculator::Roles::instructor ||
  global.userDefault.userRole == UserCalculator::Roles::teacher;
}

bool Database::User::logoutViaDatabase() {
  STACK_TRACE("Database::User::logoutViaDatabase");
  if (!global.flagLoggedIn) {
    return true;
  }
  UserCalculator user;
  user.UserCalculatorData::operator=(global.userDefault);
  user.resetAuthenticationToken(nullptr);
  global.setWebInput("authenticationToken", "");
  global.cookiesToBeSent.setKeyValue("authenticationToken", "");
  return true;
}

QueryExact::QueryExact() {}

QueryExact::QueryExact(
  const std::string& desiredCollection,
  const std::string& label,
  const std::string& desiredValue
) {
  this->collection = desiredCollection;
  this->nestedLabels.addOnTop(label);
  this->value = desiredValue;
}

QueryExact::QueryExact(
  const std::string& desiredCollection,
  const List<std::string>& desiredLabels,
  const std::string& desiredValue
) {
  this->collection = desiredCollection;
  this->value = desiredValue;
  this->nestedLabels.setSize(0);
  this->nestedLabels.addListOnTop(desiredLabels);
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

std::string QueryExact::toString() const {
  std::stringstream out;
  out
  << "collection: "
  << this->collection
  << ", query: "
  << this->toJSON().toString();
  return out.str();
}

JSData QueryExact::toJSONCombineLabelsAndValue() const {
  JSData result;
  JSData* current = &result;
  JSData unknown;
  std::string label;
  for (int i = 0; i < this->nestedLabels.size - 1; i ++) {
    label = Database::convertStringToMongoKeyString(this->nestedLabels[i]);
    (*current)[label] = unknown;
    current = &((*current)[label]);
  }
  label =
  Database::convertStringToMongoKeyString(
    *this->nestedLabels.lastObject()
  );
  (*current)[label] = this->value;
  return result;
}

JSData QueryExact::toJSON() const {
  JSData result;
  JSData encodedKeys;
  if (!Database::convertJSONToJSONMongo(this->value, encodedKeys)) {
    global
    << Logger::red
    << "Failed to convert find query to mongoDB encoding. "
    << Logger::endL;
    result.elementType = JSData::token::tokenNull;
    return result;
  }
  result.elementType = JSData::token::tokenObject;
  result[this->getLabel()] = encodedKeys;
  return result;
}

std::string QueryExact::getLabelFromNestedLabels(
  const std::string& first, const std::string& second
) {
  return
  QueryExact::getLabelFromNestedLabels(
    List<std::string>({first, second})
  );
}

std::string QueryExact::getLabelFromNestedLabels(
  const List<std::string>& nestedLabels
) {
  std::stringstream out;
  for (int i = 0; i < nestedLabels.size; i ++) {
    std::string currentKey =
    Database::convertStringToMongoKeyString(nestedLabels[i]);
    out << currentKey;
    if (i != nestedLabels.size - 1) {
      out << ".";
    }
  }
  return out.str();
}

std::string QueryExact::getLabel() const {
  return this->getLabelFromNestedLabels(this->nestedLabels);
}

bool QueryExact::getMongoKeyValue(
  std::string& outputKey, std::string& outputValue
) const {
  std::stringstream out;
  if (this->nestedLabels.size == 0) {
    return false;
  }
  out << this->collection << "." << this->getLabel();
  outputKey = out.str();
  return this->value.isString(&outputValue);
}

void QueryExact::setLabelsValue(
  const List<std::string>& labels, const std::string& desiredValue
) {
  this->value = desiredValue;
  this->nestedLabels = labels;
}

void QueryExact::setLabelValue(
  const std::string& label, const std::string& desiredValue
) {
  this->value = desiredValue;
  this->nestedLabels.setSize(1);
  this->nestedLabels[0] = label;
}

bool Database::convertJSONToJSONMongo(
  const JSData& input,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  if (&input == &output) {
    JSData inputCopy = input;
    return
    Database::convertJSONToJSONMongo(inputCopy, output, commentsOnFailure);
  }
  return
  Database::convertJSONToJSONEncodeKeys(
    input, output, 0, true, commentsOnFailure
  );
}

bool Database::convertJSONMongoToJSON(
  const JSData& input,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  if (&input == &output) {
    JSData inputCopy = input;
    return
    Database::convertJSONMongoToJSON(inputCopy, output, commentsOnFailure);
  }
  return
  Database::convertJSONToJSONEncodeKeys(
    input, output, 0, false, commentsOnFailure
  );
}

bool Database::convertJSONToJSONEncodeKeys(
  const JSData& input,
  JSData& output,
  int recursionDepth,
  bool encodeOverDecode,
  std::stringstream* commentsOnFailure
) {
  static const int maxRecursionDepth = 100;
  if (recursionDepth > maxRecursionDepth) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Input json is too deeply nested, maximum depth: "
      << maxRecursionDepth
      << ". ";
    }
    return false;
  }
  output.reset(input.elementType);
  if (input.elementType == JSData::token::tokenArray) {
    for (int i = 0; i < input.listObjects.size; i ++) {
      JSData nextItem;
      if (
        !Database::convertJSONToJSONEncodeKeys(
          input.listObjects[i],
          nextItem,
          recursionDepth + 1,
          encodeOverDecode,
          commentsOnFailure
        )
      ) {
        return false;
      }
      output.listObjects.addOnTop(nextItem);
    }
    return true;
  }
  if (input.elementType == JSData::token::tokenObject) {
    for (int i = 0; i < input.objects.size(); i ++) {
      JSData nextItem;
      if (
        !Database::convertJSONToJSONEncodeKeys(
          input.objects.values[i],
          nextItem,
          recursionDepth + 1,
          encodeOverDecode,
          commentsOnFailure
        )
      ) {
        return false;
      }
      std::string key;
      if (encodeOverDecode) {
        key =
        Database::convertStringToMongoKeyString(input.objects.keys[i]);
      } else {
        key =
        HtmlRoutines::convertURLStringToNormal(
          input.objects.keys[i], false
        );
      }
      output[key] = nextItem;
    }
    return true;
  }
  output = input;
  return true;
}

std::string Database::convertStringToMongoKeyString(
  const std::string& input
) {
  std::stringstream out;
  for (unsigned int i = 0; i < input.size(); i ++) {
    if (
      HtmlRoutines::isRepresentedByItselfInURLs(input[i]) && input[i] != '.'
    ) {
      out << input[i];
    } else {
      out << "%";
      int x = input[i];
      out << std::hex << ((x / 16) % 16) << (x % 16) << std::dec;
    }
  }
  return out.str();
}

bool Database::findOneWithOptions(
  const QueryExact& query,
  const QueryResultOptions& options,
  JSData& output,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  STACK_TRACE("Database::findOneWithOptions");
  (void) commentsGeneralNonSensitive;
  if (global.flagDatabaseCompiled) {
    return
    Database::get().mongoDB.findOneWithOptions(
      query,
      options,
      output,
      commentsOnFailure,
      commentsGeneralNonSensitive
    );
  } else {
    // We are using the fallback database
    if (options.toJSON().objects.size() > 0) {
      *commentsOnFailure
      << "Project compiled without mongoDB support, "
      << "so I am using a fallback database. "
      << "Non-empty query options are not supported: "
      << options.toJSON();
      return false;
    }
    return Database::fallBack.findOne(query, output, commentsOnFailure);
  }
}

bool Database::findOne(
  const QueryExact& query,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  QueryResultOptions emptyOptions;
  return
  this->findOneWithOptions(
    query, emptyOptions, output, commentsOnFailure, nullptr
  );
}

bool Database::updateOne(
  const QueryExact& findQuery,
  const QuerySet& dataToMerge,
  std::stringstream* commentsOnFailure
) {
  if (findQuery.isEmpty()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty query not allowed";
    }
    return false;
  }
  if (global.flagDatabaseCompiled) {
    return this->mongoDB.updateOne(findQuery, dataToMerge, commentsOnFailure);
  } else if (!global.flagDisableDatabaseLogEveryoneAsAdmin) {
    return
    this->fallBack.updateOne(findQuery, dataToMerge, commentsOnFailure);
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "Database::updateOne fail: no DB compiled and fallback is disabled. ";
  }
  return false;
}

bool Database::findOneFromSome(
  const List<QueryExact>& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  if (global.flagDatabaseCompiled) {
    return
    this->mongoDB.findOneFromSome(findOrQueries, output, commentsOnFailure);
  } else if (!global.flagDisableDatabaseLogEveryoneAsAdmin) {
    return
    this->fallBack.findOneFromSome(findOrQueries, output, commentsOnFailure);
  } else {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Database::findOneFromSome failed. "
      << DatabaseStrings::errorDatabaseDisabled;
    }
    return false;
  }
}

bool Database::deleteDatabase(std::stringstream* commentsOnFailure) {
  if (global.flagDatabaseCompiled) {
    return this->mongoDB.deleteDatabase(commentsOnFailure);
  } else {
    return this->fallBack.deleteDatabase(commentsOnFailure);
  }
}

void Database::createHashIndex(
  const std::string& collectionName, const std::string& key
) {
  STACK_TRACE("Database::createHashIndex");
  if (global.flagDatabaseCompiled) {
    this->mongoDB.createHashIndex(collectionName, key);
  } else if (!global.flagDisableDatabaseLogEveryoneAsAdmin) {
    this->fallBack.createHashIndex(collectionName, key);
  }
}

bool Database::initializeWorker() {
  STACK_TRACE("Database::initializeWorker");
  if (this->flagInitializedWorker) {
    return true;
  }
  if (global.flagDatabaseCompiled) {
    this->mongoDB.initialize();
  }
  this->createHashIndex(
    DatabaseStrings::tableUsers, DatabaseStrings::labelUsername
  );
  this->createHashIndex(
    DatabaseStrings::tableUsers, DatabaseStrings::labelEmail
  );
  this->createHashIndex(
    DatabaseStrings::tableUsers, DatabaseStrings::labelInstructor
  );
  this->createHashIndex(
    DatabaseStrings::tableUsers, DatabaseStrings::labelUserRole
  );
  this->createHashIndex(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelProblemDataJSON +
    "." +
    DatabaseStrings::labelProblemFileName
  );
  this->createHashIndex(
    DatabaseStrings::tableProblemInformation,
    DatabaseStrings::labelProblemFileName
  );
  this->createHashIndex(
    DatabaseStrings::tableDeleted, DatabaseStrings::labelUsername
  );
  this->flagInitializedWorker = true;
  return true;
}

bool Database::initializeServer() {
  STACK_TRACE("Database::initializeServer");
  this->user.owner = this;
  this->fallBack.owner = this;
  this->flagInitializedServer = true;
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    return true;
  }
  if (global.flagDatabaseCompiled) {
    return true;
  }
  global
  << Logger::red
  << "Calculator compiled without (mongoDB) database support. "
  << Logger::green
  << "Using "
  << Logger::red
  << "**SLOW** "
  << Logger::green
  << "fall-back JSON storage."
  << Logger::endL;
  this->fallBack.initialize();
  return true;
}

void GlobalVariables::initialize() {
  this->logs.worker.fileName =
  "/LogFiles/logs/" +
  GlobalVariables::getDateForLogFiles() +
  "/logCommon.html";
  this->logs.server.fileName =
  "/LogFiles/logs/" + GlobalVariables::getDateForLogFiles() + "/global.html";
  this->logs.serverMonitor.fileName =
  "/LogFiles/logs/" + GlobalVariables::getDateForLogFiles() + "/global.html";
}

void GlobalVariables::initModifiableDatabaseFields() {
  STACK_TRACE("GlobalVariables::initModifiableDatabaseFields");
  List<List<std::string> >& modifiableData = global.databaseModifiableFields;
  List<std::string> currentEntry;
  modifiableData.reserve(10);
  currentEntry.addOnTop(DatabaseStrings::tableUsers);
  currentEntry.addOnTop(DatabaseStrings::objectSelectoR);
  currentEntry.addOnTop(DatabaseStrings::labelProblemDataJSON);
  currentEntry.addOnTop(DatabaseStrings::anyFielD);
  modifiableData.addOnTop(currentEntry);
  currentEntry.setSize(0);
  currentEntry.addOnTop(DatabaseStrings::tableUsers);
  currentEntry.addOnTop(DatabaseStrings::objectSelectoR);
  modifiableData.addOnTop(currentEntry);
  currentEntry.setSize(0);
  currentEntry.addOnTop(DatabaseStrings::tableEmailInfo);
  currentEntry.addOnTop(DatabaseStrings::objectSelectoR);
  modifiableData.addOnTop(currentEntry);
  std::fstream outputFile;
  FileOperations::openFileCreateIfNotPresentVirtual(
    outputFile,
    "/calculator_html/modifiable_database_fields.js",
    false,
    true,
    false
  );
  JSData modifiableDataJSON;
  modifiableDataJSON = modifiableData;
  JSData toWrite;
  toWrite["modifiableFields"] = modifiableDataJSON;
  outputFile << "//File automatically generated. Please do not modify.\n";
  outputFile << "\"use strict\";\n";
  outputFile
  << "var modifiableDatabaseData = "
  << toWrite.toString(&JSData::PrintOptions::newLine())
  << ";\n";
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

std::string ProblemData::toStringAvailableAnswerIds() {
  std::stringstream out;
  out << "Available answer ids: ";
  for (int i = 0; i < this->answers.size(); i ++) {
    out << this->answers.values[i].answerId;
    if (i != this->answers.size() - 1) {
      out << ", ";
    }
  }
  return out.str();
}

bool ProblemDataAdministrative::getWeightFromCourse(
  const std::string& courseNonURLed,
  Rational& output,
  std::string* outputAsGivenByInstructor
) {
  STACK_TRACE("ProblemDataAdministrative::getWeightFromCourse");
  if (!this->problemWeightsPerCourse.contains(courseNonURLed)) {
    return false;
  }
  std::string tempString;
  if (outputAsGivenByInstructor == nullptr) {
    outputAsGivenByInstructor = &tempString;
  }
  *outputAsGivenByInstructor =
  this->problemWeightsPerCourse.getValueCreateEmpty(courseNonURLed);
  return output.assignStringFailureAllowed(*outputAsGivenByInstructor);
}

std::string ProblemDataAdministrative::toString() const {
  std::stringstream out;
  out << this->deadlinesPerSection.toStringHtml();
  out << this->problemWeightsPerCourse.toStringHtml();
  return out.str();
}

bool ProblemData::checkConsistency() const {
  STACK_TRACE("ProblemData::checkConsistency");
  for (int i = 0; i < this->answers.size(); i ++) {
    if (
      StringRoutines::stringTrimWhiteSpace(
        this->answers.values[i].answerId
      ) ==
      ""
    ) {
      global.fatal
      << "This is not supposed to happen: empty answer id."
      << global.fatal;
    }
  }
  return true;
}

bool ProblemData::checkConsistencyMathQuillIds() const {
  STACK_TRACE("ProblemData::checkConsistencyMathQuillIds");
  for (int i = 0; i < this->answers.size(); i ++) {
    if (
      StringRoutines::stringTrimWhiteSpace(
        this->answers.values[i].idMathEquationField
      ) ==
      ""
    ) {
      std::stringstream errorStream;
      errorStream
      << "This is not supposed to happen: empty idMQField. The answer id is: "
      << this->answers.values[i].answerId
      << "<br>"
      << this->toString()
      << "<hr>Answer information: "
      << this->toString()
      << "<br>";
      global.fatal << errorStream.str() << global.fatal;
    }
  }
  return true;
}

std::string ProblemData::toString() const {
  std::stringstream out;
  out << "Problem data. " << "Random seed: " << this->randomSeed;
  if (this->flagRandomSeedGiven) {
    out << " (given)";
  }
  out << ". <br>";
  for (int i = 0; i < this->answers.size(); i ++) {
    Answer& currentA = this->answers.values[i];
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
  out << "Administrator data: " << this->adminData.toString();
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
  for (
    unsigned i = 0; i < this->usernameHashedPlusPassWordHashed.size(); i ++
  ) {
    this->usernameHashedPlusPassWordHashed[i] = ' ';
  }
  for (
    unsigned i = 0; i < this->enteredHashedSaltedPassword.size(); i ++
  ) {
    this->enteredHashedSaltedPassword[i] = ' ';
  }
  for (
    unsigned i = 0; i < this->actualHashedSaltedPassword.size(); i ++
  ) {
    this->actualHashedSaltedPassword[i] = ' ';
  }
}

std::string UserCalculator::toStringSelectedColumns() {
  STACK_TRACE("UserCalculator::toStringSelectedColumns");
  std::stringstream out;
  out << this->selectedColumnsUnsafe.size << " columns selected. ";
  for (int i = 0; i < this->selectedColumnsUnsafe.size; i ++) {
    out
    << "<br>"
    << this->selectedColumnsUnsafe[i]
    << ": "
    << this->selectedColumnsUnsafe[i]
    << this->selectedColumnsRetrievalFailureRemarks[i];
  }
  return out.str();
}

std::string UserCalculator::toString() {
  STACK_TRACE("UserCalculator::toString");
  std::stringstream out;
  out
  << "Calculator user: "
  << this->username
  << "<br>Section: computed: "
  << this->sectionComputed
  << ", in DB: "
  << this->sectionInDB
  << "<br>Sections taught: "
  << this->sectionsTaught.toStringCommaDelimited()
  << "<br>Course: computed: "
  << this->courseInDB
  << ", in DB: "
  << this->courseComputed;
  Rational weightRat;
  for (int i = 0; i < this->problemData.size(); i ++) {
    out
    << "<br>Problem: "
    << this->problemData.keys[i]
    << "; random seed: "
    << this->problemData.values[i].randomSeed
    << "; numSubmissions: "
    << this->problemData.values[i].totalNumSubmissions
    << "; correct: "
    << this->problemData.values[i].numCorrectlyAnswered
    << "; points: "
    << this->problemData.values[i].points
    << ";";
    if (
      !this->problemData.values[i].adminData.getWeightFromCourse(
        this->courseComputed, weightRat
      )
    ) {
      out << " (weight not available). ";
    } else {
      out << " weight: " << weightRat.toString();
    }
  }
  out
  << "<br>Deadline info: "
  << HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(
    this->deadlines.toString(nullptr)
  );
  return out.str();
}

bool UserCalculator::fetchOneColumn(
  const std::string& columnNameUnsafe,
  std::string& outputUnsafe,
  std::stringstream* failureComments
) {
  STACK_TRACE("UserCalculator::fetchOneColumn");
  (void) columnNameUnsafe;
  (void) outputUnsafe;
  (void) failureComments;
  global.fatal << "not implemented" << global.fatal;
  return false;
}

bool UserCalculator::authenticateWithToken(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("UserCalculator::authenticateWithToken");
  (void) commentsOnFailure;
  if (this->enteredAuthenticationToken == "") {
    return false;
  }
  TimeWrapper now;
  now.assignLocalTime();
  this->approximateHoursSinceLastTokenWasIssued =
  now.subtractAnotherTimeFromMeAndGet_APPROXIMATE_ResultInHours(
    this->authenticationCreationTime
  );
  // <-to do: revoke authentication token if expired.
  return this->enteredAuthenticationToken == this->actualAuthenticationToken;
}

bool UserCalculator::loadFromDatabase(
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  STACK_TRACE("UserCalculator::loadFromDatabase");
  (void) commentsGeneral;
  (void) commentsOnFailure;
  double startTime = global.getElapsedSeconds();
  if (
    !Database::get().user.loadUserInformation(*this, commentsOnFailure)
  ) {
    return false;
  }
  this->computeCourseInformation();
  if (this->deadlineSchema != "") {
    QueryExact findDeadlines(
      DatabaseStrings::tableDeadlines,
      DatabaseStrings::labelDeadlinesSchema,
      this->deadlineSchema
    );
    JSData outDeadlinesQuery;
    if (
      Database::get().findOne(
        findDeadlines, outDeadlinesQuery, commentsGeneral
      )
    ) {
      this->deadlines = outDeadlinesQuery[DatabaseStrings::labelDeadlines];
    }
  }
  if (this->problemWeightSchema != "") {
    QueryExact findProblemWeights(
      DatabaseStrings::tableProblemWeights,
      DatabaseStrings::labelProblemWeightsSchema,
      this->problemWeightSchema
    );
    JSData outProblemWeightsQuery;
    if (
      Database::get().findOne(
        findProblemWeights, outProblemWeightsQuery, commentsGeneral
      )
    ) {
      this->problemWeights =
      outProblemWeightsQuery[DatabaseStrings::labelProblemWeight];
    }
  }
  global.timeStats["userLoadTime"] = global.getElapsedSeconds() - startTime;
  return true;
}

bool UserCalculatorData::loadFromJSON(JSData& input) {
  STACK_TRACE("UserCalculatorData::loadFromJSON");
  this->userId = input[DatabaseStrings::labelUserId].stringValue;
  this->username = input[DatabaseStrings::labelUsername].stringValue;
  this->email = input[DatabaseStrings::labelEmail].stringValue;
  this->actualActivationToken =
  input[DatabaseStrings::labelActivationToken].stringValue;
  this->timeOfActivationTokenCreation =
  input[DatabaseStrings::labelTimeOfActivationTokenCreation].stringValue;
  this->actualAuthenticationToken =
  input[DatabaseStrings::labelAuthenticationToken].stringValue;
  this->timeOfAuthenticationTokenCreation =
  input[DatabaseStrings::labelTimeOfAuthenticationTokenCreation].stringValue;
  this->problemDataJSON = input[DatabaseStrings::labelProblemDataJSON];
  this->actualHashedSaltedPassword =
  input[DatabaseStrings::labelPassword].stringValue;
  this->userRole = input[DatabaseStrings::labelUserRole].stringValue;
  this->instructorInDB = input[DatabaseStrings::labelInstructor].stringValue;
  this->semesterInDB = input[DatabaseStrings::labelSemester].stringValue;
  this->sectionInDB = input[DatabaseStrings::labelSection].stringValue;
  this->courseInDB = input[DatabaseStrings::labelCurrentCourses].stringValue;
  this->sectionsTaught.setSize(0);
  // TODO(tmilev): Remove URL decoding.
  // The layer of URL decoding is here because older
  // versions of the calculator stored pass hashes as:
  // percentEncode(base64Encode(raw_bytes)).
  // The current version stores without % encoding:
  // base64Encode(raw_bytes).
  // Since the password hash is 32 bytes, its base64 encoding ends in =
  // which in turn means passwords stored by older version of the calculator
  // have wrong endings.
  this->actualHashedSaltedPassword =
  HtmlRoutines::convertURLStringToNormal(
    this->actualHashedSaltedPassword, false
  );
  JSData sectionsTaughtList = input[DatabaseStrings::labelSectionsTaught];
  if (sectionsTaughtList.elementType == JSData::token::tokenArray) {
    for (int i = 0; i < sectionsTaughtList.listObjects.size; i ++) {
      this->sectionsTaught.addOnTop(
        sectionsTaughtList.listObjects[i].stringValue
      );
    }
  }
  return true;
}

JSData UserCalculatorData::toJSON() {
  STACK_TRACE("UserCalculatorData::toJSON");
  JSData result;
  result[DatabaseStrings::labelUserId] = this->userId;
  result[DatabaseStrings::labelUsername] = this->username;
  result[DatabaseStrings::labelEmail] = this->email;
  result[DatabaseStrings::labelActivationToken] = this->actualActivationToken;
  result[DatabaseStrings::labelTimeOfActivationTokenCreation] =
  this->timeOfActivationTokenCreation;
  result[DatabaseStrings::labelAuthenticationToken] =
  this->actualAuthenticationToken;
  result[DatabaseStrings::labelTimeOfAuthenticationTokenCreation] =
  this->timeOfAuthenticationTokenCreation;
  result[DatabaseStrings::labelProblemDataJSON] = this->problemDataJSON;
  if (
    this->problemDataJSON.elementType == JSData::token::tokenUndefined
  ) {
    result[DatabaseStrings::labelProblemDataJSON].elementType =
    JSData::token::tokenObject;
    result[DatabaseStrings::labelProblemDataJSON].objects.clear();
  }
  result[DatabaseStrings::labelPassword] = this->actualHashedSaltedPassword;
  result[DatabaseStrings::labelUserRole] = this->userRole;
  result[DatabaseStrings::labelInstructor] = this->instructorInDB;
  result[DatabaseStrings::labelSemester] = this->semesterInDB;
  result[DatabaseStrings::labelSection] = this->sectionInDB;
  result[DatabaseStrings::labelCurrentCourses] = this->courseInDB;
  JSData sectionsTaughtList;
  sectionsTaughtList.elementType = JSData::token::tokenArray;
  for (int i = 0; i < this->sectionsTaught.size; i ++) {
    sectionsTaughtList[i] = this->sectionsTaught[i];
  }
  result[DatabaseStrings::labelSectionsTaught] = sectionsTaughtList;
  for (int i = result.objects.size() - 1; i >= 0; i --) {
    JSData& currentValue = result.objects.values[i];
    if (
      currentValue.stringValue == "" &&
      currentValue.elementType == JSData::token::tokenString
    ) {
      result.objects.removeKey(result.objects.keys[i]);
    }
  }
  return result;
}

bool UserCalculatorData::computeCourseInformation() {
  STACK_TRACE("UserCalculatorData::computeCourseInformation");
  bool isAdmin = (
    this->userRole == UserCalculator::Roles::administator &&
    this->username == global.userDefault.username
  );
  if (
    global.userStudentVieWOn() &&
    isAdmin &&
    global.getWebInput("studentSection") != ""
  ) {
    // <- warning, the user may not be
    // fully logged-in yet so global.userDefaultHasAdminRights()
    // does not work right.
    // TODO(tmilev): fix this by computing course information later when
    // needed.
    this->sectionComputed =
    HtmlRoutines::convertURLStringToNormal(
      global.getWebInput("studentSection"), false
    );
  } else {
    this->sectionComputed = this->sectionInDB;
  }
  if (isAdmin && global.getWebInput(WebAPI::problem::courseHome) != "") {
    this->courseComputed =
    HtmlRoutines::convertURLStringToNormal(
      global.getWebInput(WebAPI::problem::courseHome), false
    );
  } else {
    this->courseComputed = this->courseInDB;
  }
  if (isAdmin) {
    this->instructorComputed = this->username;
  } else {
    this->instructorComputed = this->instructorInDB;
  }
  this->deadlineSchema =
  "deadlines" +
  this->instructorComputed +
  this->courseComputed +
  this->semesterComputed;
  this->problemWeightSchema = "problemWeights" + this->instructorComputed;
  return true;
}

void UserCalculator::setProblemData(
  const std::string& problemName, const ProblemData& inputData
) {
  STACK_TRACE("UserCalculator::setProblemData");
  this->problemData.setKeyValue(problemName, inputData);
}

std::string ProblemData::store() {
  STACK_TRACE("ProblemData::store");
  std::stringstream out;
  if (this->flagRandomSeedGiven) {
    out << "randomSeed=" << this->randomSeed;
  }
  for (int i = 0; i < this->answers.size(); i ++) {
    Answer& currentA = this->answers.values[i];
    if (this->flagRandomSeedGiven || i != 0) {
      out << "&";
    }
    out
    << HtmlRoutines::convertStringToURLString(currentA.answerId, false)
    << "=";
    std::stringstream questionsStream;
    questionsStream
    << "numCorrectSubmissions="
    << currentA.numCorrectSubmissions
    << "&numSubmissions="
    << currentA.numSubmissions
    << "&firstCorrectAnswer="
    << HtmlRoutines::convertStringToURLString(
      currentA.firstCorrectAnswerClean, false
    );
    out
    << HtmlRoutines::convertStringToURLString(
      questionsStream.str(), false
    );
  }
  return out.str();
}

JSData ProblemData::storeJSON() const {
  STACK_TRACE("ProblemData::storeJSON");
  JSData result;
  result.elementType = JSData::token::tokenObject;
  if (this->flagRandomSeedGiven) {
    std::stringstream stringConverter;
    stringConverter << this->randomSeed;
    // Store random seed as string to avoid type conversion issues.
    result[WebAPI::problem::randomSeed] = stringConverter.str();
  }
  for (int i = 0; i < this->answers.size(); i ++) {
    Answer& currentA = this->answers.values[i];
    JSData currentAnswerJSON;
    currentAnswerJSON["numCorrectSubmissions"] =
    std::to_string(currentA.numCorrectSubmissions);
    currentAnswerJSON["numSubmissions"] =
    std::to_string(currentA.numSubmissions);
    currentAnswerJSON["firstCorrectAnswer"] =
    HtmlRoutines::convertStringToURLString(
      currentA.firstCorrectAnswerClean, false
    );
    result[
      HtmlRoutines::convertStringToURLString(currentA.answerId, false)
    ] =
    currentAnswerJSON;
  }
  return result;
}

bool UserCalculator::shouldCommentOnMissingUser() {
  if (this->username.size() < 4) {
    return true;
  }
  return
  this->username == WebAPI::userDefaultAdmin ||
  this->username == "admin" ||
  this->username == "guest" ||
  this->username == "student" ||
  this->username == "teacher";
}

std::string UserCalculator::firstLoginMessage() {
  std::stringstream out;
  if (global.flagRequestComingLocally) {
    out
    << "If this is your first run, set the username to "
    << "default and enter the password you desire. "
    << "The password will then be automatically set. "
    << "To add further accounts login as default and go to 'Accounts'. ";
  }
  return out.str();
}

bool UserCalculator::authenticate(std::stringstream* commentsOnFailure) {
  STACK_TRACE("UserCalculator::authenticate");
  std::stringstream secondCommentsStream;
  if (!this->loadFromDatabase(&secondCommentsStream)) {
    if (commentsOnFailure != nullptr) {
      if (this->shouldCommentOnMissingUser()) {
        *commentsOnFailure << "User " << this->username << " does not exist. ";
        *commentsOnFailure << this->firstLoginMessage();
      }
      if (global.flagDebugLogin) {
        *commentsOnFailure << secondCommentsStream.str();
      }
    }
    return false;
  }
  if (this->authenticateWithToken(&secondCommentsStream)) {
    return true;
  }
  bool result = this->authenticateWithPassword(commentsOnFailure);
  if (this->enteredPassword != "") {
    this->resetAuthenticationToken(commentsOnFailure);
  }
  // <- this needs to be fixed: an attacker may cause denial of service by
  // launching fake login attempts.
  return result;
}

bool UserCalculator::authenticateWithPassword(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("UserCalculator::authenticateWithPassword");
  (void) commentsOnFailure;
  this->computeHashedSaltedPassword();
  // TODO(tmilev): timing attacks on the comparison may be used to guess the
  // length of the salted hashed password. Fix this with a proper HMAC
  // comparison.
  return this->enteredHashedSaltedPassword == this->actualHashedSaltedPassword;
}

bool UserCalculator::resetAuthenticationToken(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("UserCalculator::resetAuthenticationToken");
  TimeWrapper now;
  now.assignLocalTime();
  std::stringstream out;
  List<unsigned char> authenticationToken;
  Crypto::Random::getRandomBytesSecureInternalMayLeaveTracesInMemory(
    authenticationToken, 20
  );
  this->actualAuthenticationToken =
  Crypto::convertListUnsignedCharsToBase64(authenticationToken, true);
  QueryExact findUser(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelUsername,
    this->username
  );
  QuerySet setUser;
  setUser.value[DatabaseStrings::labelAuthenticationToken] =
  this->actualAuthenticationToken;
  setUser.value[DatabaseStrings::labelTimeOfAuthenticationTokenCreation] =
  now.timeStringNonReadable;
  Database::get().updateOne(findUser, setUser, commentsOnFailure);
  this->flagNewAuthenticationTokenComputedUserNeedsIt = true;
  return true;
}

bool UserCalculator::setPassword(std::stringstream* commentsOnFailure) {
  STACK_TRACE("UserCalculator::setPassword");
  if (this->enteredPassword == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty password not allowed. ";
    }
    return false;
  }
  this->computeHashedSaltedPassword();
  QueryExact findUser(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelUsername,
    this->username
  );
  QuerySet setUser;
  setUser.value[DatabaseStrings::labelPassword] =
  this->enteredHashedSaltedPassword;
  return Database::get().updateOne(findUser, setUser, commentsOnFailure);
}

bool UserCalculator::isAcceptableCharDatabaseInput(char character) {
  if (MathRoutines::isDigit(character)) {
    return true;
  }
  if (MathRoutines::isLatinLetter(character)) {
    return true;
  }
  switch (character) {
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

bool UserCalculator::isAcceptableDatabaseInput(
  const std::string& input, std::stringstream* comments
) {
  STACK_TRACE("UserCalculator::IsAcceptableDatabaseInput");
  for (unsigned i = 0; i < input.size(); i ++) {
    if (!UserCalculator::isAcceptableCharDatabaseInput(input[i])) {
      if (comments != nullptr) {
        *comments
        << "Input: "
        << input
        << " contains at least one invalid character: "
        << input[i]
        << ".";
      }
      return false;
    }
  }
  return true;
}

std::string UserCalculator::getSelectedRowEntry(const std::string& key) {
  STACK_TRACE("UserCalculator::GetSelectedRowEntry");
  int index = this->selectedRowFieldNamesUnsafe.getIndex(key);
  if (index == - 1) {
    return "";
  }
  return this->selectedRowFieldsUnsafe[index];
}

bool Database::User::sendActivationEmail(
  const std::string& emailList,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsGeneralSensitive
) {
  STACK_TRACE("Database::User::sendActivationEmail");
  List<std::string> emails;
  StringRoutines::stringSplitDefaultDelimiters(emailList, emails);
  return
  Database::User::sendActivationEmail(
    emails, commentsOnFailure, commentsGeneral, commentsGeneralSensitive
  );
}

bool Database::User::sendActivationEmail(
  const List<std::string>& emails,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsGeneralSensitive
) {
  STACK_TRACE("Database::User::sendActivationEmail");
  UserCalculator currentUser;
  bool result = true;
  for (int i = 0; i < emails.size; i ++) {
    global
    << "Sending activation email, user "
    << i + 1
    << " out of "
    << emails.size
    << " ... ";
    currentUser.username = emails[i];
    currentUser.email = emails[i];
    global.server().getActiveWorker().doSetEmail(
      currentUser,
      commentsOnFailure,
      commentsGeneral,
      commentsGeneralSensitive
    );
  }
  if (commentsOnFailure != nullptr) {
    global << commentsOnFailure->str();
  }
  if (commentsGeneral != nullptr && commentsOnFailure != commentsGeneral) {
    global << commentsGeneral->str();
  }
  if (
    commentsGeneralSensitive != nullptr &&
    commentsGeneralSensitive != commentsOnFailure
  ) {
    global << commentsGeneralSensitive->str();
  }
  return result;
}

bool ProblemData::loadFromOldFormat(
  const std::string& inputData, std::stringstream& commentsOnFailure
) {
  STACK_TRACE("ProblemData::loadFromOldFormat");
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > mapStrings;
  if (
    !HtmlRoutines::chopPercentEncodedString(
      inputData, mapStrings, commentsOnFailure
    )
  ) {
    return false;
  }
  this->points = 0;
  this->numCorrectlyAnswered = 0;
  this->totalNumSubmissions = 0;
  this->flagRandomSeedGiven = false;
  if (global.userRequestRequiresLoadingRealExamData()) {
    if (mapStrings.contains(WebAPI::problem::randomSeed)) {
      global.comments << "Loading random seed from old format.";
      this->randomSeed = static_cast<uint32_t>(
        atoi(
          mapStrings.getValueCreateEmpty(WebAPI::problem::randomSeed).c_str()
        )
      );
      this->flagRandomSeedGiven = true;
    }
  }
  this->answers.clear();
  bool result = true;
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > currentQuestionMap;
  for (int i = 0; i < mapStrings.size(); i ++) {
    if (mapStrings.keys[i] == WebAPI::problem::randomSeed) {
      continue;
    }
    Answer answer;
    answer.answerId = mapStrings.keys[i];
    this->answers.setKeyValue(answer.answerId, answer);
    Answer& currentA = *this->answers.values.lastObject();
    std::string currentQuestion =
    HtmlRoutines::convertURLStringToNormal(mapStrings.values[i], false);
    result =
    HtmlRoutines::chopPercentEncodedString(
      currentQuestion, currentQuestionMap, commentsOnFailure
    );
    if (!result) {
      commentsOnFailure
      << "Failed to interpret as key-value pair: "
      << currentQuestion
      << ". ";
      continue;
    }
    if (currentQuestionMap.contains("numCorrectSubmissions")) {
      currentA.numCorrectSubmissions =
      atoi(
        currentQuestionMap.getValueCreateEmpty("numCorrectSubmissions").c_str()
      );
    }
    if (currentQuestionMap.contains("numSubmissions")) {
      currentA.numSubmissions =
      atoi(
        currentQuestionMap.getValueCreateEmpty("numSubmissions").c_str()
      );
    }
    if (currentQuestionMap.contains("firstCorrectAnswer")) {
      currentA.firstCorrectAnswerURLed =
      currentQuestionMap.getValueCreateEmpty("firstCorrectAnswer");
      currentA.firstCorrectAnswerClean =
      HtmlRoutines::convertURLStringToNormal(
        currentA.firstCorrectAnswerURLed, false
      );
      currentA.firstCorrectAnswerURLed =
      HtmlRoutines::convertStringToURLString(
        currentA.firstCorrectAnswerClean, false
      );
      // url-encoding back the cleaned up answer:
      // this protects from the possibility that
      // currentA.firstCorrectAnswerURLed was not encoded properly.
    }
  }
  //  this->checkConsistency();
  return result;
}

bool ProblemData::loadFromJSON(
  const JSData& inputData, std::stringstream& commentsOnFailure
) {
  STACK_TRACE("ProblemData::loadFromJSON");
  (void) commentsOnFailure;
  this->points = 0;
  this->numCorrectlyAnswered = 0;
  this->totalNumSubmissions = 0;
  this->flagRandomSeedGiven = false;
  this->randomSeed = - 1;
  if (global.userRequestRequiresLoadingRealExamData()) {
    if (inputData.objects.contains(WebAPI::problem::randomSeed)) {
      this->randomSeed = static_cast<uint32_t>(
        atoi(
          inputData.objects.getValueNoFail(WebAPI::problem::randomSeed).
          stringValue.c_str()
        )
      );
      this->flagRandomSeedGiven = true;
    }
  }
  this->answers.clear();
  bool result = true;
  for (int i = 0; i < inputData.objects.size(); i ++) {
    if (inputData.objects.keys[i] == WebAPI::problem::randomSeed) {
      continue;
    }
    Answer newAnswer;
    newAnswer.answerId = inputData.objects.keys[i];
    this->answers.setKeyValue(newAnswer.answerId, newAnswer);
    Answer& currentA = *this->answers.values.lastObject();
    JSData currentQuestionJSON = inputData.objects.values[i];
    if (currentQuestionJSON.objects.contains("numCorrectSubmissions")) {
      currentA.numCorrectSubmissions =
      atoi(
        currentQuestionJSON.objects.getValueNoFail("numCorrectSubmissions").
        stringValue.c_str()
      );
    }
    if (currentQuestionJSON.objects.contains("numSubmissions")) {
      currentA.numSubmissions =
      atoi(
        currentQuestionJSON.objects.getValueNoFail("numSubmissions").
        stringValue.c_str()
      );
    }
    if (currentQuestionJSON.objects.contains("firstCorrectAnswer")) {
      currentA.firstCorrectAnswerURLed =
      currentQuestionJSON.objects.getValueNoFail("firstCorrectAnswer").
      stringValue;
      currentA.firstCorrectAnswerClean =
      HtmlRoutines::convertURLStringToNormal(
        currentA.firstCorrectAnswerURLed, false
      );
      currentA.firstCorrectAnswerURLed =
      HtmlRoutines::convertStringToURLString(
        currentA.firstCorrectAnswerClean, false
      );
      // url-encoding back the cleaned up answer:
      // this protects from the possibility that
      // currentA.firstCorrectAnswerURLed was not encoded properly,
      // say, by an older version of the calculator
    }
  }
  //  this->checkConsistency();
  return result;
}

bool UserCalculator::interpretDatabaseProblemData(
  const std::string& information, std::stringstream& commentsOnFailure
) {
  STACK_TRACE("UserCalculator::interpretDatabaseProblemData");
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > mapStrings;
  if (
    !HtmlRoutines::chopPercentEncodedString(
      information, mapStrings, commentsOnFailure
    )
  ) {
    return false;
  }
  this->problemData.clear();
  this->problemData.setExpectedSize(mapStrings.size());
  bool result = true;
  ProblemData reader;
  std::string probNameNoWhiteSpace;
  for (int i = 0; i < mapStrings.size(); i ++) {
    if (
      !reader.loadFromOldFormat(
        HtmlRoutines::convertURLStringToNormal(
          mapStrings.values[i], false
        ),
        commentsOnFailure
      )
    ) {
      result = false;
      continue;
    }
    probNameNoWhiteSpace =
    StringRoutines::stringTrimWhiteSpace(
      HtmlRoutines::convertURLStringToNormal(mapStrings.keys[i], false)
    );
    if (probNameNoWhiteSpace == "") {
      continue;
    }
    this->problemData.setKeyValue(probNameNoWhiteSpace, reader);
  }
  return result;
}

bool UserCalculator::interpretDatabaseProblemDataJSON(
  const JSData& data, std::stringstream& commentsOnFailure
) {
  STACK_TRACE("UserCalculator::interpretDatabaseProblemDataJSON");
  this->problemData.clear();
  this->problemData.setExpectedSize(data.objects.size());
  bool result = true;
  ProblemData reader;
  std::string problemNameNoWhiteSpace;
  for (int i = 0; i < data.objects.size(); i ++) {
    if (
      !reader.loadFromJSON(data.objects.values[i], commentsOnFailure)
    ) {
      result = false;
      continue;
    }
    problemNameNoWhiteSpace =
    StringRoutines::stringTrimWhiteSpace(data.objects.keys[i]);
    if (problemNameNoWhiteSpace == "") {
      continue;
    }
    this->problemData.setKeyValue(problemNameNoWhiteSpace, reader);
  }
  return result;
}

bool UserCalculator::exists(std::stringstream* comments) {
  STACK_TRACE("UserCalculator::exists");
  JSData notUsed;
  return
  Database::get().findOneFromSome(
    this->getFindMeFromUserNameQuery(), notUsed, comments
  );
}

bool UserCalculator::computeAndStoreActivationToken(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("UserCalculator::computeAndStoreActivationToken");
  TimeWrapper now;
  now.assignLocalTime();
  List<unsigned char> activationToken;
  Crypto::Random::getRandomBytesSecureInternalMayLeaveTracesInMemory(
    activationToken, 16
  );
  this->actualActivationToken =
  Crypto::convertListUnsignedCharsToBase64(activationToken, true);
  QueryExact findUserQuery(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelUsername,
    this->username
  );
  QuerySet updateUser;
  updateUser.value[DatabaseStrings::labelActivationToken] =
  this->actualActivationToken;
  if (
    !Database::get().updateOne(findUserQuery, updateUser, commentsOnFailure)
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Setting activation token failed. ";
    }
    this->actualActivationToken = "";
    return false;
  }
  return true;
}

bool UserCalculator::computeAndStoreActivationEmailAndTokens(
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  STACK_TRACE("UserCalculator::computeAndStoreActivationEmailAndTokens");
  if (!this->computeAndStoreActivationToken(commentsOnFailure)) {
    return false;
  }
  return
  this->computeAndStoreActivationStats(commentsOnFailure, commentsGeneral);
}

bool UserCalculator::computeAndStoreActivationStats(
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  STACK_TRACE("UserCalculator::computeAndStoreActivationStats");
  if (EmailRoutines::webAdress == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The web address is missing: "
      << "the administrator of this instance of the calculator needs to "
      << "set up entry webAddress in file configuraiton/configuration.json. ";
    }
    return false;
  }
  std::string activationAddress;
  if (
    !this->getActivationAddressFromActivationToken(
      this->actualActivationToken,
      this->username,
      this->email,
      activationAddress,
      nullptr
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to compute your activation url. ";
    }
    return false;
  }
  QueryExact findEmail(
    DatabaseStrings::tableEmailInfo,
    DatabaseStrings::labelEmail,
    this->email
  );
  JSData emailStat;
  if (
    !Database::get().findOne(findEmail, emailStat, commentsOnFailure)
  ) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "No recorded previous attempts to activate this email. ";
    }
  }
  std::string lastEmailTime, emailCountForThisEmail;
  lastEmailTime =
  emailStat[DatabaseStrings::labelLastActivationEmailTime].stringValue;
  emailCountForThisEmail =
  emailStat[DatabaseStrings::labelNumActivationEmails].stringValue;
  LargeInteger numActivationsThisEmail = 0;
  if (emailCountForThisEmail != "") {
    numActivationsThisEmail.assignString(emailCountForThisEmail);
  }
  numActivationsThisEmail ++;
  TimeWrapper now;
  TimeWrapper lastActivationOnThisEmail;
  TimeWrapper lastActivationOnThisAccount;
  now.assignLocalTime();
  if (lastEmailTime != "") {
    lastActivationOnThisEmail.operator=(lastEmailTime);
    lastActivationOnThisAccount.operator=(
      this->timeOfActivationTokenCreation
    );
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "Last activation on this email, GM time: "
      << lastActivationOnThisEmail.toStringGM()
      << ". "
      << "Last activation on this account, GM time: "
      << lastActivationOnThisEmail.toStringGM()
      << ". ";
    }
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral
    << "Total activations (attempted on) this email, "
    << "including the present one: "
    << numActivationsThisEmail.toString()
    << ". ";
  }
  QueryExact findQueryInUsers(DatabaseStrings::tableUsers, "", "");
  QuerySet updateUser;
  updateUser.value[DatabaseStrings::labelTimeOfActivationTokenCreation] =
  now.toString();
  if (this->userId != "") {
    findQueryInUsers.setLabelValue(
      DatabaseStrings::labelUserId, this->userId
    );
  } else if (this->username != "") {
    findQueryInUsers.setLabelValue(
      DatabaseStrings::labelUsername, this->username
    );
  } else {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "This shouldn't happen: "
      << "both the username and the user id are empty. ";
    }
    return false;
  }
  if (
    !Database::get().updateOne(
      findQueryInUsers, updateUser, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to set activationTokenCreationTime. ";
    }
    return false;
  }
  QuerySet emailStatQuery;
  emailStatQuery.value[DatabaseStrings::labelLastActivationEmailTime] =
  now.toString();
  emailStatQuery.value[DatabaseStrings::labelNumActivationEmails] =
  numActivationsThisEmail.toString();
  emailStatQuery.value[DatabaseStrings::labelActivationToken] =
  this->actualActivationToken;
  emailStatQuery.value[DatabaseStrings::labelUsername] = this->username;
  emailStatQuery.value[DatabaseStrings::labelEmail] = this->email;
  if (
    !Database::get().updateOne(findEmail, emailStatQuery, commentsOnFailure)
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unexpected failure to update email stats. ";
    }
    return false;
  }
  this->activationEmailSubject = "NO REPLY: Activation of your math account. ";
  if (global.flagDebugLogin && !global.flagDatabaseCompiled) {
    global.comments
    << "Activation link displayed for debugging purposes. Database is off. "
    << "<a href='"
    << activationAddress
    << "'>"
    << activationAddress
    << "</a>";
  }
  std::stringstream emailBody;
  emailBody
  << "Dear user,"
  << "\n\nto confirm your email change at our website "
  << EmailRoutines::webAdress
  << ", please follow the activation link below.\n\n "
  << activationAddress
  << " \n\nSincerely, \nthe "
  << EmailRoutines::webAdress
  << " team";
  this->activationEmail = emailBody.str();
  return true;
}

List<QueryExact> UserCalculatorData::getFindMeFromUserNameQuery() const {
  List<QueryExact> result;
  if (this->username != "") {
    QueryExact findByUsername(
      DatabaseStrings::tableUsers,
      DatabaseStrings::labelUsername,
      this->username
    );
    result.addOnTop(findByUsername);
  }
  if (this->email != "") {
    QueryExact findByEmail(
      DatabaseStrings::tableUsers,
      DatabaseStrings::labelEmail,
      this->email
    );
    result.addOnTop(findByEmail);
  }
  if (result.size == 0) {
    global.fatal
    << "User with find query not allowed to have neither username nor email. "
    << global.fatal;
  }
  return result;
}

bool UserCalculator::storeProblemData(
  const std::string& fileName, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("UserCalculator::storeProblemData");
  if (!this->problemData.contains(fileName)) {
    global.fatal
    << "I was asked to store fileName: "
    << fileName
    << " but I have no record of it in my problem data map. "
    << global.fatal;
  }
  const ProblemData& problem = this->problemData.getValueNoFail(fileName);
  QuerySet update;
  std::string key =
  QueryExact::getLabelFromNestedLabels(
    DatabaseStrings::labelProblemDataJSON, fileName
  );
  update.value[key] = problem.storeJSON();
  return
  Database::get().updateOneFromSome(
    this->getFindMeFromUserNameQuery(), update, commentsOnFailure
  );
}

bool Database::User::addUsersFromEmails(
  const std::string& emailList,
  const std::string& userPasswords,
  std::string& userRole,
  std::string& userGroup,
  std::stringstream& comments,
  int& outputNumNewUsers,
  int& outputNumUpdatedUsers
) {
  STACK_TRACE("Database::User::addUsersFromEmails");
  global.millisecondsMaxComputation = 100000;
  // 100 seconds
  global.millisecondsReplyAfterComputation = 200000;
  // 200 seconds
  List<std::string> emails, passwords;
  StringRoutines::stringSplitDefaultDelimiters(emailList, emails);
  StringRoutines::stringSplitDefaultDelimiters(userPasswords, passwords);
  if (passwords.size > 0) {
    if (passwords.size != emails.size) {
      comments
      << "Different number of usernames/emails and passwords: "
      << emails.size
      << " emails and "
      << passwords.size
      << " passwords. "
      << "If you want to enter usernames without password, "
      << "you need to leave the password input box empty. ";
      return false;
    }
  }
  UserCalculator currentUser;
  bool result = true;
  bool doSendEmails = false;
  outputNumNewUsers = 0;
  outputNumUpdatedUsers = 0;
  for (int i = 0; i < emails.size; i ++) {
    currentUser.reset();
    currentUser.username = emails[i];
    currentUser.courseInDB =
    global.getWebInput(WebAPI::problem::courseHome);
    currentUser.sectionInDB = userGroup;
    currentUser.instructorInDB = global.userDefault.username;
    currentUser.email = emails[i];
    currentUser.userRole = userRole;
    bool isEmail = true;
    if (emails[i].find('@') == std::string::npos) {
      isEmail = false;
      currentUser.email = "";
    } else {
      currentUser.email = emails[i];
    }
    QuerySet currentUserData;
    List<QueryExact> findUser;
    findUser.addOnTop(
      QueryExact(
        DatabaseStrings::tableUsers,
        DatabaseStrings::labelUsername,
        currentUser.username
      )
    );
    findUser.addOnTop(
      QueryExact(
        DatabaseStrings::tableUsers,
        DatabaseStrings::labelEmail,
        currentUser.email
      )
    );
    if (
      !this->owner->findOneFromSome(
        findUser, currentUserData.value, &comments
      )
    ) {
      if (!currentUser.exists(&comments)) {
        if (!currentUser.storeToDatabase(false, &comments)) {
          comments << "Failed to create user: " << currentUser.username;
          result = false;
          continue;
        } else {
          outputNumNewUsers ++;
        }
      }
      if (isEmail) {
        this->owner->updateOneFromSome(findUser, currentUserData, &comments);
      }
    } else {
      outputNumUpdatedUsers ++;
      // currentUser may have its updated entries modified by the functions
      // above.
      QuerySet setUser;
      setUser.value = currentUser.toJSON();
      if (
        !this->owner->updateOneFromSome(findUser, setUser, &comments)
      ) {
        result = false;
      }
    }
    if (passwords.size == 0 || passwords.size != emails.size) {
      if (
        currentUser.actualHashedSaltedPassword == "" &&
        currentUser.actualAuthenticationToken == ""
      ) {
        if (!currentUser.computeAndStoreActivationToken(&comments)) {
          result = false;
        }
      }
    } else {
      currentUser.enteredPassword =
      HtmlRoutines::convertStringToURLString(passwords[i], false);
      // <-Passwords are ONE-LAYER url-encoded
      // <-INCOMING pluses in passwords MUST be decoded as spaces, this is how
      // form.submit() works!
      // <-Incoming pluses must be re-coded as spaces (%20).
      if (!currentUser.setPassword(&comments)) {
        result = false;
      }
      JSData activatedJSON;
      activatedJSON[DatabaseStrings::labelActivationToken] = "activated";
      this->owner->updateOneFromSome(
        findUser, QuerySet::makeFrom(activatedJSON), &comments
      );
      if (currentUser.email != "") {
        currentUser.computeAndStoreActivationStats(&comments, &comments);
      }
    }
  }
  if (!result) {
    comments << "<br>Failed to create all users. ";
  }
  if (doSendEmails) {
    std::stringstream* commentsGeneralSensitive = nullptr;
    if (global.userDefaultHasAdminRights()) {
      commentsGeneralSensitive = &comments;
    }
    if (
      !Database::User::sendActivationEmail(
        emails, &comments, &comments, commentsGeneralSensitive
      )
    ) {
      result = false;
    }
  }
  return result;
}

bool UserCalculator::getActivationAddress(
  std::string& output, std::stringstream& comments
) {
  STACK_TRACE("UserCalculator::getActivationAddress");
  if (!this->loadFromDatabase(&comments, &comments)) {
    return false;
  }
  this->actualActivationToken = this->getSelectedRowEntry("activationToken");
  if (this->actualActivationToken == "") {
    comments
    << "Failed to fetch activation token for user: "
    << this->username;
    return false;
  }
  if (this->actualActivationToken == "activated") {
    comments << "Account of user: " << this->username << "already activated";
    return false;
  }
  return
  this->getActivationAddressFromActivationToken(
    this->actualActivationToken,
    this->username,
    this->email,
    output,
    nullptr
  );
}

bool EmailRoutines::sendEmailWithMailGun(
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsGeneralSensitive
) {
  STACK_TRACE("EmailRoutines::sendEmailWithMailGun");
  if (this->sendEmailFrom == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Emails have not been setup on this instance of the calculator. "
      << "The website admin needs to set field: "
      << "sendEmailFrom inside file configuration/configuration.json. ";
    }
    return false;
  }
  std::string mailGunKey;
  if (
    !FileOperations::
    loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
      "certificates/mailgun-api.txt",
      mailGunKey,
      true,
      true,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Could not find mailgun key. "
      << "The key must be located in file: \n"
      << "certificates/mailgun-api.txt\n"
      << " The file must be uploaded manually to the server. ";
    }
    return false;
  }
  if (mailGunKey.size() > 0) {
    mailGunKey.resize(mailGunKey.size() - 1);
  }
  global
  << "Sending email via "
  << "https://api.mailgun.net/v3/mail2."
  << EmailRoutines::sendEmailFrom
  << "/messages "
  << Logger::endL;
  std::stringstream commandToExecute;
  commandToExecute << "curl -s --user 'api:" << mailGunKey << "' ";
  commandToExecute
  << "https://api.mailgun.net/v3/"
  << EmailRoutines::sendEmailFrom
  << "/messages ";
  commandToExecute
  << "-F from='Automated Email "
  << "<noreply@mail2."
  << EmailRoutines::sendEmailFrom
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
  // WARNING: calling command line
  // that includes user inputs.
  // Those MUST be sanitized (else an attacker would use username with "
  // characters
  // to execute arbitrary code.
  commandToExecute
  << "-F to=\""
  << HtmlRoutines::convertStringEscapeQuotesAndBackslashes(this->toEmail)
  << "\" "
  << "-F subject=\""
  << HtmlRoutines::convertStringEscapeQuotesAndBackslashes(this->subject)
  << "\" "
  << "-F text=\""
  << HtmlRoutines::convertStringEscapeQuotesAndBackslashes(
    this->emailContent
  )
  << "\"";
  if (global.flagDebugLogin) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "Since this is a login debug, "
      << "I am not sending your activation email. ";
      global.comments
      << "<br><b>Email command:</b><br>"
      << commandToExecute.str()
      << "<br>";
    }
    return true;
  }
  std::string commandResult =
  global.externalCommandReturnOutput(commandToExecute.str());
  global << commandResult << Logger::endL;
  bool success = commandResult.find("Forbidden") == std::string::npos;
  if (!success && commentsOnFailure != nullptr) {
    int start =
    MathRoutines::maximum(
      0, static_cast<int>(mailGunKey.size() - 17)
    );
    std::string apiKey = mailGunKey.substr(start);
    *commentsOnFailure
    << "Failed to send your email from our automated account: "
    << EmailRoutines::sendEmailFrom
    << ". This is an error in our web site's email setup. "
    << "If you can, please take a screenshot and file us a bug report! "
    << "Mailgun api key: "
    << apiKey
    << ". ";
  }
  if (commentsGeneralSensitive != nullptr) {
    *commentsGeneralSensitive << "Result: ";
    *commentsGeneralSensitive
    << HtmlRoutines::convertStringToHtmlString(commandResult, true);
  }
  return success;
}

List<bool> EmailRoutines::recognizedEmailCharacters;
List<bool>& EmailRoutines::getRecognizedEmailChars() {
  if (recognizedEmailCharacters.size == 0) {
    recognizedEmailCharacters.initializeFillInObject(256, false);
    std::string characters =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    characters += "0123456789";
    characters += "@";
    characters += "!#$%&'*+-/=?.";
    for (unsigned i = 0; i < characters.size(); i ++) {
      recognizedEmailCharacters[
        static_cast<unsigned char>(characters[i])
      ] =
      true;
    }
  }
  return EmailRoutines::recognizedEmailCharacters;
}

bool EmailRoutines::isOKEmail(
  const std::string& input, std::stringstream* commentsOnError
) {
  STACK_TRACE("EmailRoutines::isOKEmail");
  if (input.size() == 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Empty email not allowed. ";
    }
    return false;
  }
  int numAts = 0;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (
      !EmailRoutines::getRecognizedEmailChars()[
        static_cast<unsigned char>(input[i])
      ]
    ) {
      if (commentsOnError != nullptr) {
        *commentsOnError
        << "Email: "
        << input
        << " contains the unrecognized character "
        << input[i]
        << ". ";
      }
      return false;
    }
    if (input[i] == '@') {
      numAts ++;
    }
  }
  if (numAts != 1 && commentsOnError != nullptr) {
    *commentsOnError
    << "Email: "
    << input
    << " is required to have exactly one @ character. ";
  }
  return numAts == 1;
}

EmailRoutines::EmailRoutines() {
  this->smtpWithPort = "smtp.gmail.com:587";
}

bool Database::User::loginViaGoogleTokenCreateNewAccountIfNeeded(
  UserCalculatorData& user,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral,
  bool& tokenIsGood
) {
  (void) commentsOnFailure;
  (void) user;
  (void) commentsGeneral;
  if (!global.flagDatabaseCompiled) {
    return Database::User::loginNoDatabaseSupport(user, commentsGeneral);
  }
  STACK_TRACE("Database::User::loginViaGoogleTokenCreateNewAccountIfNeeded");
  UserCalculator userWrapper;
  userWrapper.::UserCalculatorData::operator=(user);
  if (userWrapper.enteredGoogleToken == "") {
    return false;
  }
  if (
    !Crypto::verifyJWTagainstKnownKeys(
      userWrapper.enteredGoogleToken, commentsOnFailure, commentsGeneral
    )
  ) {
    return false;
  }
  tokenIsGood = true;
  JSONWebToken webToken;
  if (
    !webToken.assignString(
      userWrapper.enteredGoogleToken, commentsOnFailure
    )
  ) {
    return false;
  }
  JSData data;
  if (!data.parse(webToken.claimsJSON, commentsOnFailure)) {
    return false;
  }
  if (
    data.getValue("email").elementType != JSData::token::tokenString
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Could not find email entry in the json data "
      << data.toString(nullptr);
    }
    return false;
  }
  userWrapper.email = data.getValue("email").stringValue;
  userWrapper.username = "";
  if (!userWrapper.exists(commentsOnFailure)) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "User with email "
      << userWrapper.email
      << " does not exist. ";
    }
    userWrapper.username = userWrapper.email;
    if (!userWrapper.storeToDatabase(false, commentsOnFailure)) {
      return false;
    }
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "Created new user with username: "
      << userWrapper.username;
    }
    user = userWrapper;
    return true;
  }
  if (!userWrapper.loadFromDatabase(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to fetch user with email "
      << userWrapper.email
      << ". ";
    }
    return false;
  }
  if (userWrapper.actualAuthenticationToken == "") {
    userWrapper.resetAuthenticationToken(commentsOnFailure);
  }
  user = userWrapper;
  return true;
}

bool Database::User::loginNoDatabaseSupport(
  UserCalculatorData& user, std::stringstream* commentsGeneral
) {
  if (global.flagDatabaseCompiled) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      <<
      "Database support is available, yet you requested no-database login. "
      <<
      "The command 'make -j10 noMongo=1' compiles the calculator without database support."
      ;
    }
    return false;
  }
  if (!global.flagDisableDatabaseLogEveryoneAsAdmin) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "Logging-in without database is not allowed. "
      << "To allow, set "
      << Configuration::disableDatabaseLogEveryoneAsAdmin
      << " to true in the server's configuration.json file. ";
    }
    return false;
  }
  // When the database is disabled, we assume the user is an administrator.
  // The purpose of this is that
  // users who do not have a mongoDB installed
  // (or have troubles accessing it for some reason)
  // can still use the administrator functions of the calculator, for example,
  // modify problem files from the one-page app.
  user.userRole = UserCalculatorData::Roles::administator;
  user.actualAuthenticationToken = "compiledWithoutDatabaseSupport";
  if (commentsGeneral != nullptr) {
    *commentsGeneral
    << "Automatic login as default: calculator compiled without DB. ";
  }
  return true;
}

bool Database::User::loginViaDatabase(
  UserCalculatorData& user, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::User::loginViaDatabase");
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    return Database::User::loginNoDatabaseSupport(user, commentsOnFailure);
  }
  UserCalculator userWrapper;
  userWrapper.::UserCalculatorData::operator=(user);
  if (userWrapper.authenticate(commentsOnFailure)) {
    user = userWrapper;
    return true;
  }
  if (
    userWrapper.enteredAuthenticationToken != "" &&
    userWrapper.enteredAuthenticationToken != "0" &&
    commentsOnFailure != nullptr
  ) {
    *commentsOnFailure
    << "<b> Authentication of user: "
    << userWrapper.username
    << " with token "
    << userWrapper.enteredAuthenticationToken
    << " failed. </b>";
  }
  if (userWrapper.enteredActivationToken != "") {
    if (
      global.requestType == WebAPI::request::changePassword ||
      global.requestType == WebAPI::request::changePasswordPage ||
      global.requestType == WebAPI::request::activateAccountJSON
    ) {
      if (
        userWrapper.actualActivationToken != "activated" &&
        userWrapper.actualActivationToken != "" &&
        userWrapper.actualActivationToken != "error"
      ) {
        if (
          userWrapper.enteredActivationToken ==
          userWrapper.actualActivationToken
        ) {
          user = userWrapper;
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
        *commentsOnFailure
        << "Activation token entered but the user request type: "
        << global.requestType
        << " does not allow login with activation token. ";
      }
    }
  }
  return this->firstLoginOfAdmin(user, userWrapper, commentsOnFailure);
}

bool Database::User::firstLoginOfAdmin(
  UserCalculatorData& incoming,
  UserCalculator& userInDatabase,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::User::firstLoginOfAdmin");
  if (userInDatabase.username != WebAPI::userDefaultAdmin) {
    return false;
  }
  if (userInDatabase.enteredPassword == "") {
    return false;
  }
  if (userInDatabase.exists(nullptr)) {
    return false;
  }
  // We have admin login.
  // The admin user does not exist in the database.
  // This is the first login, so let's set the admin account.
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "<b>First login of user default "
    << "(= default administator account): setting password.</b> ";
  }
  global
  << Logger::yellow
  << "First login of user default: setting password."
  << Logger::endL;
  userInDatabase.actualActivationToken = "activated";
  userInDatabase.userRole = UserCalculator::Roles::administator;
  if (!userInDatabase.storeToDatabase(true, commentsOnFailure)) {
    global << Logger::red << "Failed to store default's pass to database. ";
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to store default's pass to database. ";
      global << commentsOnFailure->str();
    }
  }
  incoming = userInDatabase;
  return true;
}

void UserCalculator::computeHashedSaltedPassword() {
  STACK_TRACE("UserCalculator::computeHashedSaltedPassword");
  this->usernameHashedPlusPassWordHashed.resize(
    Crypto::LengthSha3DefaultInBytes* 2
  );
  List<unsigned char> hasher;
  Crypto::computeSha3_256(this->enteredPassword, hasher);
  // <-careful copying entered password around. We want to avoid leaving
  // passwords in non-zeroed memory, even if properly
  // freed (to the extent possible and practical).
  for (unsigned i = 0; i < Crypto::LengthSha3DefaultInBytes; i ++) {
    this->usernameHashedPlusPassWordHashed[
      i + Crypto::LengthSha3DefaultInBytes
    ] =
    static_cast<char>(hasher[i]);
  }
  Crypto::computeSha3_256(this->username, hasher);
  for (unsigned i = 0; i < Crypto::LengthSha3DefaultInBytes; i ++) {
    this->usernameHashedPlusPassWordHashed[i] = static_cast<char>(hasher[i]);
  }
  this->enteredHashedSaltedPassword =
  Crypto::computeSha3_256OutputBase64URL(
    this->usernameHashedPlusPassWordHashed
  );
}

bool UserCalculator::storeToDatabase(
  bool doSetPassword, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("UserCalculator::storeToDatabase");
  QueryExact findUser(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelUsername,
    this->username
  );
  if (this->enteredPassword != "" && doSetPassword) {
    this->computeHashedSaltedPassword();
    this->actualHashedSaltedPassword = this->enteredHashedSaltedPassword;
  }
  JSData setUser = this->toJSON();
  return
  Database::get().updateOne(
    findUser, QuerySet::makeFrom(setUser), commentsOnFailure
  );
}

bool UserCalculator::getActivationAddressFromActivationToken(
  const std::string& activationToken,
  const std::string& inputUserNameUnsafe,
  const std::string& inputEmailUnsafe,
  std::string& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("UserCalculator::getActivationAddressFromActivationToken");
  std::stringstream out;
  std::string address = EmailRoutines::webAdress;
  if (address == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Can't get activationi url: "
      << "the administrator of this instance of the calculator needs to "
      << "set up entry webAddress in file configuraiton/configuration.json. ";
    }
    return false;
  }
  if (
    !StringRoutines::stringBeginsWith(address, "https://") &&
    !StringRoutines::stringBeginsWith(address, "http://")
  ) {
    address = "https://" + address;
  }
  JSData jsonData;
  jsonData[DatabaseStrings::labelActivationToken] = activationToken;
  jsonData[DatabaseStrings::labelUsername] = inputUserNameUnsafe;
  jsonData[DatabaseStrings::labelCalculatorRequest] =
  WebAPI::request::activateAccountJSON;
  jsonData[DatabaseStrings::labelEmail] = inputEmailUnsafe;
  jsonData[DatabaseStrings::labelCurrentPage] =
  DatabaseStrings::labelPageActivateAccount;
  std::string application = global.displayApplication;
  if (global.flagDebugLogin) {
    // Use appNoCache for the link makes life easier when developing locally
    // as clicking on the link will lead you to aggressively cached
    // frontend. To get a new version of your frontend, you
    // will then have to change the url to global.displayApplicationNoCache
    // or reboot the backend, but most importantly, you may fail
    // to notice you are using a stale cache version.
    application = global.displayApplicationNoCache;
  }
  out
  << address
  << global.displayApplication
  << "#"
  << HtmlRoutines::convertStringToURLString(
    jsonData.toString(nullptr), false
  );
  output = out.str();
  return true;
}
