#include "calculator_problem_storage.h"
#include "crypto_calculator.h"
#include "database.h"
#include "html_routines.h"
#include "string_constants.h"
#include "webserver.h"

std::string UserCalculatorData::toStringFindQueries() const {
  QueryOneOfExactly queries;
  this->getFindMeQueryByUsernameOrEmail(queries);
  std::stringstream out;
  out << "userFind query: ";
  for (int i = 0; i < queries.queries.size; i ++) {
    out << queries.queries[i].toString();
    if (i != queries.queries.size - 1) {
      out << ", ";
    }
  }
  return out.str();
}

bool UserOfDatabase::setPassword(
  const std::string& inputUsername,
  const std::string& inputNewPassword,
  std::string& outputAuthenticationToken,
  std::stringstream& comments
) {
  STACK_TRACE("UserOfDatabase::setPassword");
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

bool UserOfDatabase::loadUserInformation(
  UserCalculatorData& output, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("UserOfDatabase::loadUserInformation");
  if (output.username == "" && output.email == "") {
    return false;
  }
  List<JSData> allUsers;
  QueryOneOfExactly queries;
  output.getFindMeQueryByUsernameOrEmail(queries);
  if (!this->owner->find(queries, nullptr, allUsers, nullptr)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Could not read users from the database. "
      << "The database/website must be down. ";
    }
    return false;
  }
  if (allUsers.size == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Could not find username/email. ";
      if (global.flagDebugLogin) {
        *commentsOnFailure
        << output.toStringFindQueries()
        << ". "
        << output.toStringSecure()
        << ". ";
      }
    }
    return false;
  }
  if (allUsers.size > 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Found more than one user with your username/email! "
      << "This should not be possible. "
      << "Please file a bug report at our bug tracker!";
    }
    return false;
  }
  return output.loadFromJSON(allUsers[0]);
}

bool UserOfDatabase::userDefaultHasInstructorRights() {
  STACK_TRACE("UserOfDatabase::userDefaultHasInstructorRights");
  if (global.userDefaultHasAdminRights()) {
    return true;
  }
  if (!global.flagLoggedIn) {
    return false;
  }
  return
  global.userDefault.userRole == UserCalculator::Roles::administrator ||
  global.userDefault.userRole == UserCalculator::Roles::instructor ||
  global.userDefault.userRole == UserCalculator::Roles::teacher;
}

bool UserOfDatabase::logoutViaDatabase() {
  STACK_TRACE("UserOfDatabase::logoutViaDatabase");
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

UserOfDatabase::UserOfDatabase() {
  this->owner = nullptr;
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
    << this->problemData.values[i].totalSubmissions
    << "; correct: "
    << this->problemData.values[i].totalCorrectlyAnswered
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
    bool found =
    Database::get().findExactlyOne(
      findDeadlines, nullptr, outDeadlinesQuery, commentsGeneral
    );
    if (found) {
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
    bool found =
    Database::get().findExactlyOne(
      findProblemWeights, nullptr, outProblemWeightsQuery, commentsGeneral
    );
    if (found) {
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
  if (sectionsTaughtList.elementType == JSData::Type::tokenArray) {
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
  if (this->problemDataJSON.elementType == JSData::Type::tokenUndefined) {
    result[DatabaseStrings::labelProblemDataJSON].elementType =
    JSData::Type::tokenObject;
    result[DatabaseStrings::labelProblemDataJSON].objects.clear();
  }
  result[DatabaseStrings::labelPassword] = this->actualHashedSaltedPassword;
  result[DatabaseStrings::labelUserRole] = this->userRole;
  result[DatabaseStrings::labelInstructor] = this->instructorInDB;
  result[DatabaseStrings::labelSemester] = this->semesterInDB;
  result[DatabaseStrings::labelSection] = this->sectionInDB;
  result[DatabaseStrings::labelCurrentCourses] = this->courseInDB;
  JSData sectionsTaughtList;
  sectionsTaughtList.elementType = JSData::Type::tokenArray;
  for (int i = 0; i < this->sectionsTaught.size; i ++) {
    sectionsTaughtList[i] = this->sectionsTaught[i];
  }
  result[DatabaseStrings::labelSectionsTaught] = sectionsTaughtList;
  for (int i = result.objects.size() - 1; i >= 0; i --) {
    JSData& currentValue = result.objects.values[i];
    if (
      currentValue.stringValue == "" &&
      currentValue.elementType == JSData::Type::tokenString
    ) {
      result.objects.removeKey(result.objects.keys[i]);
    }
  }
  return result;
}

bool UserCalculatorData::computeCourseInformation() {
  STACK_TRACE("UserCalculatorData::computeCourseInformation");
  bool isAdmin = (
    this->userRole == UserCalculator::Roles::administrator &&
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
  if (isAdmin && global.getWebInput(WebAPI::Problem::courseHome) != "") {
    this->courseComputed =
    HtmlRoutines::convertURLStringToNormal(
      global.getWebInput(WebAPI::Problem::courseHome), false
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

bool UserCalculator::shouldCommentOnMissingUser() {
  static bool shouldComment = true;
  if (this->username.size() >= 4 || !shouldComment) {
    return false;
  }
  shouldComment = false;
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
  setUser.addKeyValueStringPair(
    DatabaseStrings::labelAuthenticationToken,
    this->actualAuthenticationToken
  );
  setUser.addKeyValueStringPair(
    DatabaseStrings::labelTimeOfAuthenticationTokenCreation,
    now.timeStringNonReadable
  );
  Database::get().updateOne(findUser, setUser, true, commentsOnFailure);
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
  setUser.addKeyValueStringPair(
    DatabaseStrings::labelPassword, this->enteredHashedSaltedPassword
  );
  return
  Database::get().updateOne(findUser, setUser, false, commentsOnFailure);
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
  STACK_TRACE("UserCalculator::isAcceptableDatabaseInput");
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
  STACK_TRACE("UserCalculator::getSelectedRowEntry");
  int index = this->selectedRowFieldNamesUnsafe.getIndex(key);
  if (index == - 1) {
    return "";
  }
  return this->selectedRowFieldsUnsafe[index];
}

bool UserOfDatabase::sendActivationEmail(
  const std::string& emailList,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsGeneralSensitive
) {
  STACK_TRACE("UserOfDatabase::sendActivationEmail");
  List<std::string> emails;
  StringRoutines::stringSplitDefaultDelimiters(emailList, emails);
  return
  UserOfDatabase::sendActivationEmail(
    emails, commentsOnFailure, commentsGeneral, commentsGeneralSensitive
  );
}

bool UserOfDatabase::sendActivationEmail(
  const List<std::string>& emails,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsGeneralSensitive
) {
  STACK_TRACE("UserOfDatabase::sendActivationEmail");
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

void UserCalculator::exists(
  bool& outputExists, bool& databaseIsOK, std::stringstream* comments
) {
  STACK_TRACE("UserCalculator::exists");
  List<JSData> allUsers;
  QueryOneOfExactly query;
  this->getFindMeQueryByUsernameOrEmail(query);
  databaseIsOK = Database::get().find(query, nullptr, allUsers, comments);
  outputExists = allUsers.size == 1;
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
  updateUser.addKeyValueStringPair(
    DatabaseStrings::labelActivationToken, this->actualActivationToken
  );
  if (
    !Database::get().updateOne(
      findUserQuery, updateUser, true, commentsOnFailure
    )
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
  List<JSData> allEmailStats;
  bool success =
  Database::get().find(findEmail, nullptr, allEmailStats, commentsOnFailure);
  if (!success) {
    return false;
  }
  if (allEmailStats.size == 0) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "No recorded previous attempts to activate this email. ";
    }
  }
  JSData emailStat = allEmailStats[0];
  std::string lastEmailTime, emailCountForThisEmail;
  lastEmailTime =
  emailStat[DatabaseStrings::labelLastActivationEmailTime].stringValue;
  emailCountForThisEmail =
  emailStat[DatabaseStrings::labelTotalActivationEmails].stringValue;
  LargeInteger totalActivationsThisEmail = 0;
  if (emailCountForThisEmail != "") {
    totalActivationsThisEmail.assignString(emailCountForThisEmail);
  }
  totalActivationsThisEmail ++;
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
    << totalActivationsThisEmail.toString()
    << ". ";
  }
  QueryExact findQueryInUsers(DatabaseStrings::tableUsers, "", "");
  QuerySet updateUser;
  updateUser.addKeyValueStringPair(
    DatabaseStrings::labelTimeOfActivationTokenCreation, now.toString()
  );
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
      findQueryInUsers, updateUser, true, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to set activationTokenCreationTime. ";
    }
    return false;
  }
  QuerySet emailStatQuery;
  emailStatQuery.addKeyValueStringPair(
    DatabaseStrings::labelLastActivationEmailTime, now.toString()
  );
  emailStatQuery.addKeyValueStringPair(
    DatabaseStrings::labelTotalActivationEmails,
    totalActivationsThisEmail.toString()
  );
  emailStatQuery.addKeyValueStringPair(
    DatabaseStrings::labelActivationToken, this->actualActivationToken
  );
  emailStatQuery.addKeyValueStringPair(
    DatabaseStrings::labelUsername, this->username
  );
  emailStatQuery.addKeyValueStringPair(
    DatabaseStrings::labelEmail, this->email
  );
  if (
    !Database::get().updateOne(
      findEmail, emailStatQuery, true, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unexpected failure to update email stats. ";
    }
    return false;
  }
  this->activationEmailSubject = "NO REPLY: Activation of your math account. ";
  if (
    global.flagDebugLogin && global.databaseType == DatabaseType::fallback
  ) {
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

void UserCalculatorData::getFindMeQueryByUsername(QueryExact& output) const {
  STACK_TRACE("UserCalculatorData::getFindMeQueryByUsername");
  output =
  QueryExact(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelUsername,
    this->username
  );
}

void UserCalculatorData::getFindMeQueryByUsernameOrEmail(
  QueryOneOfExactly& output
) const {
  STACK_TRACE("UserCalculatorData::getFindMeQueryByUsernameOrEmail");
  output.queries.clear();
  if (this->username != "") {
    QueryExact findByUsername(
      DatabaseStrings::tableUsers,
      DatabaseStrings::labelUsername,
      this->username
    );
    output.queries.addOnTop(findByUsername);
  }
  if (this->email != "") {
    QueryExact findByEmail(
      DatabaseStrings::tableUsers,
      DatabaseStrings::labelEmail,
      this->email
    );
    output.queries.addOnTop(findByEmail);
  }
  if (output.queries.size == 0) {
    global.fatal
    << "User with find query not allowed to have neither username nor email. "
    << global.fatal;
  }
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
  update.addNestedKeyValuePair(
    List<std::string>({DatabaseStrings::labelProblemDataJSON, fileName}),
    problem.storeJSON()
  );
  QueryExact query;
  this->getFindMeQueryByUsername(query);
  return Database::get().updateOne(query, update, false, commentsOnFailure);
}

bool UserOfDatabase::addUsersFromEmails(
  const std::string& emailList,
  const std::string& userPasswords,
  std::string& userRole,
  std::string& userGroup,
  int& outputNumberOfNewUsers,
  int& outputNumberOfUpdatedUsers,
  std::stringstream* comments
) {
  STACK_TRACE("UserOfDatabase::addUsersFromEmails");
  global.millisecondsMaxComputation = 100000;
  // 100 seconds
  global.millisecondsReplyAfterComputation = 200000;
  // 200 seconds
  List<std::string> emails;
  List<std::string> passwords;
  StringRoutines::stringSplitDefaultDelimiters(emailList, emails);
  StringRoutines::stringSplitDefaultDelimiters(userPasswords, passwords);
  if (passwords.size > 0 && passwords.size != emails.size) {
    if (comments != nullptr) {
      *comments
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
  outputNumberOfNewUsers = 0;
  outputNumberOfUpdatedUsers = 0;
  for (int i = 0; i < emails.size; i ++) {
    std::string password = "";
    if (password.size() > 0) {
      password = passwords[i];
    }
    if (
      !UserOfDatabase::addOneUser(
        emails[i],
        password,
        userRole,
        userGroup,
        outputNumberOfNewUsers,
        outputNumberOfUpdatedUsers,
        comments
      )
    ) {
      return false;
    }
  }
  bool doSendEmails = false;
  bool result = true;
  if (doSendEmails) {
    std::stringstream* commentsGeneralSensitive = nullptr;
    if (global.userDefaultHasAdminRights()) {
      commentsGeneralSensitive = comments;
    }
    if (
      !UserOfDatabase::sendActivationEmail(
        emails, comments, comments, commentsGeneralSensitive
      )
    ) {
      result = false;
    }
  }
  return result;
}

bool UserOfDatabase::addOneUser(
  const std::string& userNameOrEmail,
  const std::string& password,
  std::string& userRole,
  std::string& userGroup,
  int& outputNumberOfNewUsers,
  int& outputNumberOfUpdatedUsers,
  std::stringstream* commentsOnFailure
) {
  UserCalculator currentUser;
  currentUser.username = userNameOrEmail;
  currentUser.email = userNameOrEmail;
  // If it looks like email, assume it's email.
  // The user addition comes from admin user,
  // so let's trust the input within reason.
  if (!EmailRoutines::isOKEmail(currentUser.email, commentsOnFailure)) {
    // Not an email.
    currentUser.email = "";
  }
  currentUser.courseInDB = global.getWebInput(WebAPI::Problem::courseHome);
  currentUser.sectionInDB = userGroup;
  currentUser.instructorInDB = global.userDefault.username;
  currentUser.userRole = userRole;
  JSData currentUserData;
  QueryOneOfExactly findUserByUsernameOrEmail;
  QueryExact findUserByUsername =
  QueryExact(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelUsername,
    currentUser.username
  );
  findUserByUsernameOrEmail.queries.addOnTop(findUserByUsername);
  if (currentUser.email != "") {
    findUserByUsernameOrEmail.queries.addOnTop(
      QueryExact(
        DatabaseStrings::tableUsers,
        DatabaseStrings::labelEmail,
        currentUser.email
      )
    );
  }
  List<JSData> allUsers;
  if (
    !this->owner->find(
      findUserByUsernameOrEmail, nullptr, allUsers, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Database error! ";
    }
    return false;
  }
  if (allUsers.size == 0) {
    if (!currentUser.storeToDatabase(false, commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Failed to create user: "
        << currentUser.username;
      }
      return false;
    } else {
      outputNumberOfNewUsers ++;
    }
    QuerySet setUser;
    setUser.addValue(currentUserData);
    if (currentUser.email != "") {
      this->owner->updateOne(
        findUserByUsername, setUser, true, commentsOnFailure
      );
    }
  } else {
    outputNumberOfUpdatedUsers ++;
    // currentUser may have its updated entries modified by the functions
    // above.
    QuerySet setUser;
    setUser.addValue(currentUser.toJSON());
    if (
      !this->owner->updateOne(
        findUserByUsername, setUser, false, commentsOnFailure
      )
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Failed to update user: "
        << currentUser.username;
      }
      return false;
    }
  }
  if (password == "") {
    if (
      currentUser.actualHashedSaltedPassword == "" &&
      currentUser.actualAuthenticationToken == ""
    ) {
      if (!currentUser.computeAndStoreActivationToken(commentsOnFailure)) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "Failed to store activation token: "
          << currentUser.username;
        }
        return false;
      }
    }
    return true;
  }
  currentUser.enteredPassword =
  HtmlRoutines::convertStringToURLString(password, false);
  // <-Passwords are ONE-LAYER url-encoded
  // <-INCOMING pluses in passwords MUST be decoded as spaces, this is how
  // form.submit() works!
  // <-Incoming pluses must be re-coded as spaces (%20).
  if (!currentUser.setPassword(commentsOnFailure)) {
    return false;
  }
  JSData activatedJSON;
  activatedJSON[DatabaseStrings::labelActivationToken] = "activated";
  QuerySet activatedSet;
  activatedSet.fromJSONNoFail(activatedJSON);
  if (
    !this->owner->updateOne(
      findUserByUsername, activatedSet, false, commentsOnFailure
    )
  ) {
    return false;
  }
  if (currentUser.email == "") {
    return true;
  }
  return
  currentUser.computeAndStoreActivationStats(
    commentsOnFailure, commentsOnFailure
  );
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

bool UserOfDatabase::loginViaGoogleTokenCreateNewAccountIfNeeded(
  UserCalculatorData& user,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral,
  bool& tokenIsGood
) {
  STACK_TRACE("UserOfDatabase::loginViaGoogleTokenCreateNewAccountIfNeeded");
  (void) commentsOnFailure;
  (void) user;
  (void) commentsGeneral;
  if (global.hasDisabledDatabaseEveryoneIsAdmin()) {
    return UserOfDatabase::loginNoDatabaseSupport(user, commentsGeneral);
  }
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
    data.getValue("email").elementType != JSData::Type::tokenString
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
  bool userExists = false;
  bool databaseIsOK = false;
  userWrapper.exists(userExists, databaseIsOK, commentsOnFailure);
  if (!databaseIsOK) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Database failure. ";
    }
    return false;
  }
  if (userExists) {
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

bool UserOfDatabase::loginNoDatabaseSupport(
  UserCalculatorData& user, std::stringstream* commentsGeneral
) {
  if (!global.hasDisabledDatabaseEveryoneIsAdmin()) {
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
  user.userRole = UserCalculatorData::Roles::administrator;
  user.actualAuthenticationToken = "compiledWithoutDatabaseSupport";
  if (commentsGeneral != nullptr) {
    *commentsGeneral
    << "Automatic login as default: calculator running without database. ";
  }
  return true;
}

bool UserOfDatabase::loginViaDatabase(
  UserCalculatorData& user, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("UserOfDatabase::loginViaDatabase");
  if (global.hasDisabledDatabaseEveryoneIsAdmin()) {
    return UserOfDatabase::loginNoDatabaseSupport(user, commentsOnFailure);
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
    << "Authentication of user: "
    << userWrapper.username
    << " with token "
    << userWrapper.enteredAuthenticationToken
    << " failed. ";
  }
  if (userWrapper.enteredActivationToken != "") {
    if (
      global.requestType == WebAPI::Request::changePassword ||
      global.requestType == WebAPI::Request::changePasswordPage ||
      global.requestType == WebAPI::Request::activateAccountJSON
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
          *commentsOnFailure << "Account already activated. ";
        } else {
          *commentsOnFailure << "An error during activation ocurred. ";
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

bool UserOfDatabase::firstLoginOfAdmin(
  UserCalculatorData& incoming,
  UserCalculator& userInDatabase,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("UserOfDatabase::firstLoginOfAdmin");
  if (userInDatabase.username != WebAPI::userDefaultAdmin) {
    return false;
  }
  if (userInDatabase.enteredPassword == "") {
    return false;
  }
  bool userExists = false;
  bool databaseIsOK = false;
  userInDatabase.exists(userExists, databaseIsOK, nullptr);
  if (!databaseIsOK) {
    *commentsOnFailure << "Database seems to be down. ";
    return false;
  }
  if (userExists) {
    return false;
  }
  // We have admin login.
  // The admin user does not exist in the database.
  // This is the first login, so let's set the admin account.
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "First login of user default "
    << "(= default administrator account): setting password. ";
  }
  global
  << Logger::yellow
  << "First login of user default: setting password. "
  << Logger::endL;
  userInDatabase.actualActivationToken = "activated";
  userInDatabase.userRole = UserCalculator::Roles::administrator;
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
  QuerySet doSetUser;
  doSetUser.addValue(this->toJSON());
  return
  Database::get().updateOne(findUser, doSetUser, true, commentsOnFailure);
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
  WebAPI::Request::activateAccountJSON;
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
