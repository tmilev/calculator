//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader7DatabaseInterface_MySQL.h"
#include "vpfHeader5Crypto.h"
#include "vpfHeader7DatabaseInterface_MySQL.h"
#include "webserver.h"
#include "vpfHeader4SystemFunctionsGlobalObjects.h"
#include "vpfHeader7DatabaseInterface_Mongodb.h"

ProjectInformationInstance ProjectInfoVpf8_1MySQLcpp(__FILE__, "MySQL interface. ");

bool DatabaseRoutinesGlobalFunctions::SetPassword
(const std::string& inputUsername, const std::string& inputNewPassword, std::string& outputAuthenticationToken,
 std::stringstream& comments)
{ (void) inputUsername;
  (void) inputNewPassword;
  (void) outputAuthenticationToken;
  (void) comments;
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::SetPassword");
  if (!theGlobalVariables.flagLoggedIn)
  { comments << "Changing passwords allowed for logged-in users only. ";
    return false;
  }
  UserCalculator theUser;
  theUser.username = inputUsername;
  theUser.enteredPassword = inputNewPassword;
  //stOutput << "DEBUG: setting password: " << inputNewPassword;
  bool result = theUser.SetPassword(&comments);
  theUser.ResetAuthenticationToken(&comments);
  outputAuthenticationToken = theUser.actualAuthenticationToken;
  return result;
#else
  return false;
#endif // MACRO_use_MySQL
}

bool DatabaseRoutinesGlobalFunctions::UserExists
(const std::string& inputUsername, std::stringstream& comments)
{ (void) inputUsername;
  (void) comments;
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::UserExists");
  if (!theGlobalVariables.flagLoggedIn)
    return false;
  JSData theUserQuery;
  theUserQuery[DatabaseStrings::labelUsername] = inputUsername;
  List<JSData> theUsers;
  DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON
  (DatabaseStrings::tableUsers, theUserQuery, theUsers, - 1, 0, &comments);
  return theUsers.size > 0;
#else
  return false;
#endif // MACRO_use_MySQL
}

bool DatabaseRoutinesGlobalFunctions::UserDefaultHasInstructorRights()
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::UserDefaultHasInstructorRights");
  if (theGlobalVariables.UserDefaultHasAdminRights())
    return true;
#ifdef MACRO_use_MySQL
  if (!theGlobalVariables.flagLoggedIn)
    return false;
  return theGlobalVariables.userDefault.userRole == "admin" || theGlobalVariables.userDefault.userRole == "instructor"
  || theGlobalVariables.userDefault.userRole == "teacher";
#else
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctions::LogoutViaDatabase()
{
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::LogoutViaDatabase");
  //stOutput << "<br>DEBUG: Got to logout";
  if (!theGlobalVariables.flagLoggedIn)
  { //stOutput << "but not resetting token. ";
    return true;
  }
  UserCalculator theUser;
  theUser.UserCalculatorData::operator=(theGlobalVariables.userDefault);
  //stOutput << "<hr>DEBUG: logout: resetting token ... ";
  theUser.ResetAuthenticationToken(0);
//  stOutput << "token reset!... <hr>";
  theGlobalVariables.SetWebInpuT("authenticationToken", "");
  theGlobalVariables.CookiesToSetUsingHeaders.SetKeyValue("authenticationToken", "");
  return true;
#else
  return true;
#endif
}

std::string DatabaseStrings::labelUserId = "id";
std::string DatabaseStrings::labelUsername = "username";
std::string DatabaseStrings::labelPassword = "password";
std::string DatabaseStrings::labelUserRole = "userRole";
std::string DatabaseStrings::labelProblemData = "problemData";
std::string DatabaseStrings::labelAuthenticationToken = "authenticationToken";
std::string DatabaseStrings::labelActivationToken = "activationToken";
std::string DatabaseStrings::labelTimeOfActivationTokenCreation = "activationTokenCreationTime";
std::string DatabaseStrings::labelTimeOfAuthenticationTokenCreation = "authenticationCreationTime";
std::string DatabaseStrings::labelEmail = "email";

std::string DatabaseStrings::tableUsers = "users";
std::string DatabaseStrings::theDatabaseUser = "ace";
std::string DatabaseStrings::theDatabaseName = "aceDB";
std::string DatabaseStrings::theDatabaseNameMongo = "calculator";
std::string DatabaseStrings::tableDeadlines = "deadlines";
std::string DatabaseStrings::labelDeadlines = "deadlines";

std::string DatabaseStrings::labelSection = "studentSection";
std::string DatabaseStrings::labelCurrentCourses = "currentCourses";
std::string DatabaseStrings::labelDeadlinesSchema = "deadlineSchema";
std::string DatabaseStrings::labelInstructor = "instructor";
std::string DatabaseStrings::labelSemester = "semester";

std::string DatabaseStrings::tableProblemWeights = "problemWeights";
std::string DatabaseStrings::labelProblemWeightsSchema = "problemWeightsSchema";
std::string DatabaseStrings::labelProblemWeights = "weights";

std::string DatabaseStrings::labelSectionsTaught = "sectionsTaught";

std::string DatabaseStrings::tableEmailInfo = "emailInfo";
std::string DatabaseStrings::labelLastActivationEmailTime = "lastActivationEmailTime";
std::string DatabaseStrings::labelNumActivationEmails = "numActivationEmails";
std::string DatabaseStrings::labelUsernameAssociatedWithToken = "usernameAssociatedWithToken";

List<std::string> DatabaseStrings::modifyableColumns;

List<std::string>& DatabaseStrings::GetModifyableColumnsNotThreadSafe()
{ MacroRegisterFunctionWithName("DatabaseStrings::GetModifyableColumnsNotThreadSafe");
  //if (DatabaseStrings::modifyableColumns.size == 0) //<-warning: this is not thread-safe
  //{ DatabaseStrings::modifyableColumns.AddOnTop(DatabaseStrings::labelCourseInfo);
  //}
  return DatabaseStrings::modifyableColumns;
}

ProblemData::ProblemData()
{ this->randomSeed = 0;
  this->flagRandomSeedGiven = false;
  this->numCorrectlyAnswered = 0;
  this->totalNumSubmissions = 0;
  this->flagProblemWeightIsOK = false;
}

void ProblemData::AddEmptyAnswerIdOnTop(const std::string& inputAnswerId)
{ Answer theAnswer;
  theAnswer.answerId = inputAnswerId;
  this->theAnswers.SetKeyValue(inputAnswerId, theAnswer);
}

std::string ProblemData::ToStringAvailableAnswerIds()
{ std::stringstream out;
  out << "Available answer ids: ";
  for (int i = 0; i < this->theAnswers.size(); i ++)
  { out << this->theAnswers[i].answerId;
    if (i != this->theAnswers.size() - 1)
      out << ", ";
  }
  return out.str();
}

bool ProblemDataAdministrative::GetWeightFromCoursE
(const std::string& theCourseNonURLed, Rational& output, std::string *outputAsGivenByInstructor)
{ MacroRegisterFunctionWithName("ProblemDataAdministrative::GetWeightFromSection");
  if (!this->problemWeightsPerCoursE.Contains(theCourseNonURLed))
    return false;
  std::string tempString;
  if (outputAsGivenByInstructor == 0)
    outputAsGivenByInstructor = &tempString;
  *outputAsGivenByInstructor = this->problemWeightsPerCoursE.GetValueCreate(theCourseNonURLed);
  return output.AssignStringFailureAllowed(*outputAsGivenByInstructor);
}

std::string ProblemDataAdministrative::ToString()const
{ std::stringstream out;
  out << this->deadlinesPerSection.ToStringHtml();
  out << this->problemWeightsPerCoursE.ToStringHtml();
  return out.str();
}

bool ProblemData::CheckConsistency()const
{ MacroRegisterFunctionWithName("ProblemData::CheckConsistency");
  for (int i = 0; i < this->theAnswers.size(); i ++)
  { if (MathRoutines::StringTrimWhiteSpace(this->theAnswers[i].answerId) == "")
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
  for (int i = 0; i < this->theAnswers.size(); i ++)
  { if (MathRoutines::StringTrimWhiteSpace(this->theAnswers[i].idMQfield) == "")
      crash << "This is not supposed to happen: empty idMQfield. The answer id is: "
      << this->theAnswers[i].answerId << "<br>" << this->ToString() << "<hr>All the answers are: "
      << this->ToString() << crash;
  }
  return true;
}

std::string ProblemData::ToString()const
{ std::stringstream out;
  out << "Problem data. "
  << "Random seed: " << this->randomSeed;
  if (this->flagRandomSeedGiven)
    out << " (given)";
  out << ". ";
  for (int i = 0; i < this->theAnswers.size(); i ++)
  { Answer& currentA = this->theAnswers[i];
    out << "AnswerId: " << currentA.answerId;
    out << ", numCorrectSubmissions: " << currentA.numCorrectSubmissions;
    out << ", numSubmissions: " << currentA.numSubmissions;
    out << ", firstCorrectAnswer: ";
    if (currentA.firstCorrectAnswerClean == "")
      out << "[none yet], ";
    else
      out << "[" << currentA.firstCorrectAnswerClean << "], ";
  }
  out << "Admin data: " << this->adminData.ToString();
  return out.str();
}

#ifdef MACRO_use_MySQL

std::string UserCalculator::ToStringSelectedColumns()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToStringSelectedColumns");
  std::stringstream out;
  out << this->selectedColumnsUnsafe.size << " columns selected. ";
  for (int i = 0; i < this->selectedColumnsUnsafe.size; i ++)
    out << "<br>" << this->selectedColumnsUnsafe[i] << ": " << this->selectedColumnsUnsafe[i] << this->selectedColumnsRetrievalFailureRemarks[i];
  return out.str();
}

std::string UserCalculator::ToString()
{ MacroRegisterFunctionWithName("UserCalculator::ToString");
  std::stringstream out;

  out << "Calculator user: " << this->username
  << "<br>Section: computed: " << this->sectionComputed
  << ", in DB: " << this->sectionInDB
  << "<br>Sections taught: " << this->sectionsTaught.ToStringCommaDelimited()
  << "<br>Course: computed: " << this->courseInDB << ", in DB: " << this->courseComputed;

  Rational weightRat;
  for (int i = 0; i < this->theProblemData.size(); i ++)
  { out << "<br>Problem: " << this->theProblemData.theKeys[i] << "; random seed: "
    << this->theProblemData.theValues[i].randomSeed << "; numSubmissions: "
    << this->theProblemData.theValues[i].totalNumSubmissions
    << "; correct: "
    << this->theProblemData.theValues[i].numCorrectlyAnswered
    << "; points: "
    << this->theProblemData.theValues[i].Points
    << ";";
    if (!this->theProblemData.theValues[i].adminData.GetWeightFromCoursE(this->courseComputed, weightRat))
      out << " (weight not available). ";
    else
      out << " weight: " << weightRat.ToString();
  }
  out << "<br>Deadline info: "
  << HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(this->deadlines.ToString(false));
  return out.str();
}

UserCalculator::UserCalculator()
{ this->flagNewAuthenticationTokenComputedUserNeedsIt = false;
  this->approximateHoursSinceLastTokenWasIssued = 2;
}

UserCalculator::~UserCalculator()
{ for (unsigned i = 0; i < this->enteredPassword.size(); i ++)
    this->enteredPassword[i] = ' ';
  for (unsigned i = 0; i < this->usernamePlusPassWord.size(); i ++)
    this->usernamePlusPassWord[i] = ' ';
  for (unsigned i = 0; i < this->enteredShaonedSaltedPassword.size(); i ++)
    this->enteredShaonedSaltedPassword[i] = ' ';
  for (unsigned i = 0; i < this->actualShaonedSaltedPassword.size(); i ++)
    this->actualShaonedSaltedPassword[i] = ' ';
}

bool UserCalculator::FetchOneColumn
(const std::string& columnNameUnsafe, std::string& outputUnsafe,
 std::stringstream* failureComments)
{ MacroRegisterFunctionWithName("UserCalculator::FetchOneColumn");
  (void) columnNameUnsafe;
  (void) outputUnsafe;
  (void) failureComments;
  crash << "not implemented" << crash;
  return false;
}

bool UserCalculator::AuthenticateWithToken(std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::AuthenticateWithToken");
  (void) commentsOnFailure;
//  stOutput << "<br>DEBUG: token-authenticating user: " << this->username.value << " with entered token: " << this->enteredAuthenticationToken.value
//  << " against actual token: " << this->actualAuthenticationToken.value;
  if (this->enteredAuthenticationToken == "")
    return false;
  TimeWrapper now;
  now.AssignLocalTime();
  this->approximateHoursSinceLastTokenWasIssued =
  now.SubtractAnotherTimeFromMeAndGet_APPROXIMATE_ResultInHours(this->authenticationCreationTime);
  //<-to do: revoke authentication token if expired.
  return this->enteredAuthenticationToken == this->actualAuthenticationToken;
}

bool UserCalculator::LoadFromDB(std::stringstream* failureStream, std::stringstream* commentsGeneral)
{ MacroRegisterFunctionWithName("UserCalculator::FetchOneUserRow");
  (void) failureStream;
  (void) commentsGeneral;
  if (!DatabaseRoutinesGlobalFunctionsMongo::LoadUserInfo(*this))
    return false;
  this->ComputeCourseInfo();
  //stOutput << "DEBUG:  GOT to hereE!!!";
  if (this->deadlineSchema != "")
  { //stOutput << "DEBUG: Fetching deadline schema id: " << this->courseInfo.deadlineSchemaIDComputed;
    JSData findDeadlinesQuery, outDeadlinesQuery;
    findDeadlinesQuery[DatabaseStrings::labelDeadlinesSchema] = this->deadlineSchema;
    if (DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON
        (DatabaseStrings::tableDeadlines, findDeadlinesQuery, outDeadlinesQuery, failureStream))
      this->deadlines = outDeadlinesQuery[DatabaseStrings::labelDeadlines];
  }
  //  stOutput << "DEBUG: deadlineInfo, rawest: " << reader
  //  << " this->currentCoursesInfoString:  " << this->currentCoursesInfoString.value;
  //stOutput << "DEBUG: problem info row id: " << this->problemInfoRowId.value;
  if (this->problemWeightSchema != "")
  { JSData findProblemWeightsQuery, outProblemWeightsQuery;
    //stOutput << "DEBUG: Problem weight schema: " << this->problemWeightSchema;
    findProblemWeightsQuery[DatabaseStrings::labelProblemWeightsSchema] = this->problemWeightSchema;
    if (DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON
        (DatabaseStrings::tableProblemWeights, findProblemWeightsQuery, outProblemWeightsQuery, failureStream))
      this->problemWeights = outProblemWeightsQuery[DatabaseStrings::labelProblemWeights];
    //else
    //{ stOutput << "DEBUG: Failed to find";
    //}
    //stOutput << "Final problem weight string: " << this->problemWeightString
    //<< " found with query: " << findProblemWeightsQuery.ToString(false)
    //<< " that resulted in: " << outProblemWeightsQuery.ToString(false);
  }
  return true;
}

bool UserCalculatorData::LoadFromJSON(JSData& input)
{ MacroRegisterFunctionWithName("UserCalculatorData::LoadFromJSON");
  this->userId                            = input[DatabaseStrings::labelUserId                            ].string;
  this->username                          = input[DatabaseStrings::labelUsername                          ].string;
  this->email                             = input[DatabaseStrings::labelEmail                             ].string;
  this->actualActivationToken             = input[DatabaseStrings::labelActivationToken                   ].string;
  this->timeOfActivationTokenCreation     = input[DatabaseStrings::labelTimeOfActivationTokenCreation     ].string;
  this->actualAuthenticationToken         = input[DatabaseStrings::labelAuthenticationToken               ].string;
  this->timeOfAuthenticationTokenCreation = input[DatabaseStrings::labelTimeOfAuthenticationTokenCreation ].string;
  this->problemDataString                 = input[DatabaseStrings::labelProblemData                       ].string;
  this->actualShaonedSaltedPassword       = input[DatabaseStrings::labelPassword                          ].string;
  this->userRole                          = input[DatabaseStrings::labelUserRole                          ].string;

  this->instructorInDB                    = input[DatabaseStrings::labelInstructor                        ].string;
  this->semesterInDB                      = input[DatabaseStrings::labelSemester                          ].string;
  this->sectionInDB                       = input[DatabaseStrings::labelSection                           ].string;
  this->courseInDB                        = input[DatabaseStrings::labelCurrentCourses                    ].string;
  this->sectionsTaught.SetSize(0);
  JSData sectionsTaughtList = input[DatabaseStrings::labelSectionsTaught];
  if (sectionsTaughtList.type == JSData::JSarray)
    for (int i = 0; i < sectionsTaughtList.list.size; i++)
      this->sectionsTaught.AddOnTop(sectionsTaughtList.list[i].string);
  return true;
}

JSData UserCalculatorData::ToJSON()
{ MacroRegisterFunctionWithName("UserCalculatorData::ToJSON");
  JSData result;
  result[DatabaseStrings::labelUserId                            ] = this->userId                                ;
  result[DatabaseStrings::labelUsername                          ] = this->username                              ;
  result[DatabaseStrings::labelEmail                             ] = this->email                                 ;
  result[DatabaseStrings::labelActivationToken                   ] = this->actualActivationToken                 ;
  result[DatabaseStrings::labelTimeOfActivationTokenCreation     ] = this->timeOfActivationTokenCreation         ;
  result[DatabaseStrings::labelAuthenticationToken               ] = this->actualAuthenticationToken             ;
  result[DatabaseStrings::labelTimeOfAuthenticationTokenCreation ] = this->timeOfAuthenticationTokenCreation     ;
  result[DatabaseStrings::labelProblemData                       ] = this->problemDataString                     ;
  result[DatabaseStrings::labelPassword                          ] = this->actualShaonedSaltedPassword           ;
  result[DatabaseStrings::labelUserRole                          ] = this->userRole                              ;

  result[DatabaseStrings::labelInstructor                        ] = this->instructorInDB                        ;
  result[DatabaseStrings::labelSemester                          ] = this->semesterInDB                          ;
  result[DatabaseStrings::labelSection                           ] = this->sectionInDB                           ;
  result[DatabaseStrings::labelCurrentCourses                    ] = this->courseInDB                            ;
  JSData sectionsTaughtList;
  sectionsTaughtList.type = JSData::JSarray;
  for (int i = 0; i < this->sectionsTaught.size; i ++)
    sectionsTaughtList[i] = this->sectionsTaught[i];
  result[DatabaseStrings::labelSectionsTaught] = sectionsTaughtList;
  for (int i = 0; i < result.objects.size(); i ++)
  { JSData& currentValue = result.objects.theValues[i];
    if (currentValue.string == "" && currentValue.type == JSData::JSstring)
    { result.objects.RemoveKey(result.objects.theKeys[i]);
      i --;
    }
  }
  return result;
}

bool UserCalculatorData::ComputeCourseInfo()
{ MacroRegisterFunctionWithName("UserCalculator::ComputeCourseInfo");
  bool isAdmin = (this->userRole == "admin" && this->username == theGlobalVariables.userDefault.username);
  if (theGlobalVariables.UserStudentVieWOn() && isAdmin &&
      theGlobalVariables.GetWebInput("studentSection") != "")
  //<- warning, the user may not be
  //fully logged-in yet so theGlobalVariables.UserDefaultHasAdminRights()
  //does not work right.
    this->sectionComputed = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("studentSection"), false);
  else
    this->sectionComputed = this->sectionInDB;
  if (isAdmin && theGlobalVariables.GetWebInput("courseHome") != "")
    this->courseComputed =
    HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("courseHome"), false);
  else
    this->courseComputed = this->courseInDB;
  if (isAdmin)
    this->instructorComputed = this->username;
  else
    this->instructorComputed = this->instructorInDB;
  this->deadlineSchema = "deadlines" + this->instructorComputed + this->courseComputed + this->semesterComputed;
  this->problemWeightSchema = "problemWeights" + this->instructorComputed;
  return true;
}

bool UserCalculator::Authenticate(std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  //stOutput << "<hr>DEBUG: Authenticating user: " << this->username.value << " with password: " << this->enteredPassword;
  std::stringstream secondCommentsStream;
  if (!this->LoadFromDB(&secondCommentsStream))
  { if (commentsOnFailure != 0)
    { *commentsOnFailure << "User " << this->username << " does not exist. ";
      if (theGlobalVariables.hostNoPort == "localhost" ||
          theGlobalVariables.hostNoPort == "127.0.0.1")
        *commentsOnFailure << "If this is your first run, set the username to "
        << "admin and enter the password you desire. "
        << "The password will then be automatically set. "
        << "To add further accounts login as admin and go to 'Accounts'. ";
    }
    //stOutput << "<hr>DEBUG: user " << this->username.value << " does not exist. More details: "
    //<< secondCommentsStream.str() << "<hr>";
    return false;
  }
  if (this->AuthenticateWithToken(&secondCommentsStream))
    return true;
  //*commentsOnFailure << " DEBUG: User could not authenticate with token. Actual auth token: "
  //<< this->actualShaonedSaltedPassword;
  bool result = this->AuthenticateWithUserNameAndPass(commentsOnFailure);
  if (this->enteredPassword != "")
    this->ResetAuthenticationToken(commentsOnFailure);
  //<- this needs to be fixed: an attacker may cause denial of service by launching fake login attempts.
  return result;
}

bool UserCalculator::AuthenticateWithUserNameAndPass(std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::Authenticate");
  (void) commentsOnFailure;
  this->ComputeShaonedSaltedPassword();
  std::string actualPasswordRFC4648 = this->actualShaonedSaltedPassword;
  //<-old versions of the database were using a deprecated base64 encoding
  //<- below is a small compatibility layer.
  for (unsigned i = 0; i < actualPasswordRFC4648.size(); i ++)
  { if (actualPasswordRFC4648[i] == '/')
      actualPasswordRFC4648[i] = '_';
    if (actualPasswordRFC4648[i] == '+')
      actualPasswordRFC4648[i] = '-';
  }
  //if (commentsOnFailure != 0)
  //  *commentsOnFailure << "DEBUG: Comparing " << this->enteredShaonedSaltedPassword
  //  << " with: " << actualPasswordRFC4648;
  return this->enteredShaonedSaltedPassword == actualPasswordRFC4648;
}

bool UserCalculator::Iexist(std::stringstream* comments)
{ MacroRegisterFunctionWithName("UserCalculator::Iexist");
  JSData notUsed;
  return DatabaseRoutinesGlobalFunctionsMongo::FindOneFromSome
  (DatabaseStrings::tableUsers, this->GetFindMeFromUserNameQuery(), notUsed, comments);
}

void UserCalculator::ComputeShaonedSaltedPassword()
{ MacroRegisterFunctionWithName("UserCalculator::ComputeShaonedSaltedPassword");
  this->usernamePlusPassWord = this->username;
  this->usernamePlusPassWord += this->enteredPassword;
  this->enteredShaonedSaltedPassword = Crypto::computeSha1outputBase64(this->usernamePlusPassWord);
  //<-careful copying those around. We want to avoid leaving
  //passwords in non-zeroed memory, even if properly freed (to the extent possible and practical).
  //To do: zero all buffers after use.
}

bool UserCalculator::ResetAuthenticationToken(std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::ResetAuthenticationToken");
  TimeWrapper now;
  //stOutput << "<hr>Resetting authentication token... ";
  now.AssignLocalTime();
  std::stringstream out;
  out << now.theTimeStringNonReadable << rand();
  this->actualAuthenticationToken = Crypto::computeSha1outputBase64(out.str());
  JSData findUser, setUser;
  findUser[DatabaseStrings::labelUsername] = this->username;
  setUser[DatabaseStrings::labelAuthenticationToken] = this->actualAuthenticationToken;
  setUser[DatabaseStrings::labelTimeOfAuthenticationTokenCreation] = now.theTimeStringNonReadable;
  DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(DatabaseStrings::tableUsers, findUser, setUser, commentsOnFailure);
  this->flagNewAuthenticationTokenComputedUserNeedsIt = true;
  return true;
}

bool UserCalculator::SetPassword(std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::SetPassword");
  if (this->enteredPassword == "")
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Empty password not allowed. ";
    return false;
  }
  this->ComputeShaonedSaltedPassword();
  JSData findUser, setUser;
  findUser[DatabaseStrings::labelUsername] = this->username;
  setUser[DatabaseStrings::labelPassword] = this->enteredShaonedSaltedPassword;
  return DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(DatabaseStrings::tableUsers, findUser, setUser, commentsOnFailure);
}

bool UserCalculator::IsAcceptableCharDatabaseInpuT(char theChar)
{ if (MathRoutines::isADigit(theChar))
    return true;
  if (MathRoutines::isALatinLetter(theChar))
    return true;
  switch (theChar)
  { case '.':
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
  for (unsigned i = 0; i < input.size(); i ++)
    if (!UserCalculator::IsAcceptableCharDatabaseInpuT(input[i]))
    { if (comments != 0)
        *comments << "Input: " << input << " contains at least one invalid character: " << input[i] << ".";
      return false;
    }
  return true;
}

std::string UserCalculator::GetSelectedRowEntry(const std::string& theKey)
{ MacroRegisterFunctionWithName("UserCalculator::GetSelectedRowEntry");
  int theIndex = this->selectedRowFieldNamesUnsafe.GetIndex(theKey);
  if (theIndex == - 1)
    return "";
  return this->selectedRowFieldsUnsafe[theIndex];
}

bool DatabaseRoutineS::SendActivationEmail
(const std::string& emailList, std::stringstream* commentsOnFailure,
 std::stringstream* commentsGeneral, std::stringstream* commentsGeneralSensitive)
{ MacroRegisterFunctionWithName("DatabaseRoutines::SendActivationEmail");
  List<std::string> theEmails;
  MathRoutines::StringSplitDefaultDelimiters(emailList, theEmails);
  return DatabaseRoutineS::SendActivationEmail
  (theEmails, commentsOnFailure, commentsGeneral, commentsGeneralSensitive);
}

bool UserCalculator::ComputeAndStoreActivationStats
(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral)
{ MacroRegisterFunctionWithName("UserCalculator::ComputeAndStoreActivationStats");
  std::string activationAddress = this->GetActivationAddressFromActivationToken
  (this->actualActivationToken, theGlobalVariables.hostWithPort, this->username, this->email);
  JSData findQuery, emailStatQuery;
  findQuery[DatabaseStrings::labelEmail] = this->email;
  DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON
  (DatabaseStrings::tableEmailInfo, findQuery, emailStatQuery, commentsOnFailure);

  std::string lastEmailTime, emailCountForThisEmail;
  lastEmailTime = emailStatQuery[DatabaseStrings::labelLastActivationEmailTime].string;
  emailCountForThisEmail = emailStatQuery[DatabaseStrings::labelNumActivationEmails].string;

  LargeInt numActivationsThisEmail = 0;
  if (emailCountForThisEmail != "")
    numActivationsThisEmail.AssignString(emailCountForThisEmail);
  numActivationsThisEmail ++;
  TimeWrapper now, lastActivationOnThisEmail, lastActivationOnThisAccount;
  now.AssignLocalTime();
  if (lastEmailTime != "")
  { lastActivationOnThisEmail.operator=(lastEmailTime);
    lastActivationOnThisAccount.operator=(this->timeOfActivationTokenCreation);
    if (commentsGeneral != 0)
      *commentsGeneral
      << "<br>Last activation on this email: "
      << lastActivationOnThisEmail.ToStringHumanReadable() << ".\n"
      << "<br>Last activation on this account: "
      << lastActivationOnThisEmail.ToStringHumanReadable() << ".\n";
  }
  if (commentsGeneral != 0)
    *commentsGeneral
    << "<br>Total activations (attempted on) this email: "
    << numActivationsThisEmail.ToString() << ".\n <br>\n";
  JSData findQueryInUsers, setQueryInUsers;
  setQueryInUsers[DatabaseStrings::labelTimeOfActivationTokenCreation] = now.ToString();
  if (this->userId != "")
    findQueryInUsers[DatabaseStrings::labelUserId] = this->userId;
  else if (this->username != "")
    findQueryInUsers[DatabaseStrings::labelUsername] = this->username;
  else
  { if (commentsOnFailure != 0)
      *commentsOnFailure
      << "This shouldn't happen: both the username and the user id are empty. ";
    return false;
  }
  if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON
      (DatabaseStrings::tableUsers, findQueryInUsers, setQueryInUsers, commentsOnFailure))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Failed to set activationTokenCreationTime. ";
    return false;
  }
  emailStatQuery[DatabaseStrings::labelLastActivationEmailTime] = now.ToString();
  emailStatQuery[DatabaseStrings::labelNumActivationEmails] = numActivationsThisEmail.ToString();
  emailStatQuery[DatabaseStrings::labelActivationToken] = this->actualActivationToken;
  emailStatQuery[DatabaseStrings::labelUsernameAssociatedWithToken] = this->username;
  if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(DatabaseStrings::tableEmailInfo, findQuery, emailStatQuery, commentsOnFailure))
    return false;
  //stOutput << "DEBUG: Got to here. ";
  this->activationEmailSubject = "NO REPLY: Activation of your Math homework account. ";
  std::stringstream emailBody;
  emailBody << "Dear user,"
  << "\n\nto confirm your email change at our website "
  << theGlobalVariables.hostWithPort
  << ", please follow the activation link below.\n\n "
  << activationAddress
  << "\n\nSincerely, \nthe calculator-algebra.org team";
  this->activationEmail = emailBody.str();
  return true;
}

bool UserCalculator::ComputeAndStoreActivationEmailAndTokens
(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral)
{ MacroRegisterFunctionWithName("UserCalculator::ComputeAndStoreActivationEmailAndTokens");
  if (!this->ComputeAndStoreActivationToken(commentsOnFailure))
    return false;
  return this->ComputeAndStoreActivationStats(commentsOnFailure, commentsGeneral);
}

bool UserCalculator::ComputeAndStoreActivationToken(std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::ComputeAndStoreActivationToken");
  TimeWrapper now;
  //stOutput << "Resetting authentication token. ";
  now.AssignLocalTime();
  std::stringstream activationTokenStream;
  activationTokenStream << now.theTimeStringNonReadable << rand();
  this->actualActivationToken = Crypto::computeSha1outputBase64(activationTokenStream.str());
  JSData findUserQuery, setUserQuery;
  findUserQuery[DatabaseStrings::labelUsername] = this->username;
  setUserQuery[DatabaseStrings::labelActivationToken] = this->actualActivationToken;
  if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(DatabaseStrings::tableUsers, findUserQuery, setUserQuery, commentsOnFailure))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Setting activation token failed.";
    this->actualActivationToken = "";
    return false;
  }
  return true;
}

extern WebServer theWebServer;

bool DatabaseRoutineS::SendActivationEmail
(const List<std::string>& theEmails,
 std::stringstream* commentsOnFailure,
 std::stringstream* commentsGeneral,
 std::stringstream* commentsGeneralSensitive)
{ MacroRegisterFunctionWithName("DatabaseRoutines::SendActivationEmail");
  UserCalculator currentUser;
  bool result = true;
  for (int i = 0; i < theEmails.size; i ++)
  { logEmail << "Sending activation email, user "
    << i + 1 << " out of " << theEmails.size << " ... ";
    currentUser.username = theEmails[i];
    currentUser.email = theEmails[i];
    theWebServer.GetActiveWorker().DoSetEmail
    (currentUser, commentsOnFailure, commentsGeneral, commentsGeneralSensitive);
  }
  if (commentsOnFailure != 0)
    logEmail << commentsOnFailure->str();
  if (commentsGeneral != 0 && commentsOnFailure != commentsGeneral)
    logEmail << commentsGeneral->str();
  if (commentsGeneralSensitive != 0 && commentsGeneralSensitive != commentsOnFailure)
    logEmail << commentsGeneralSensitive->str();
  return result;
}

ProblemData& UserCalculator::GetProblemDataAddIfNotPresent(const std::string& problemName)
{ MacroRegisterFunctionWithName("UserCalculator::GetProblemDataAddIfNotPresent");
  return this->theProblemData.GetValueCreate(problemName);
}

void UserCalculator::SetProblemData(const std::string& problemName, const ProblemData& inputData)
{ MacroRegisterFunctionWithName("UserCalculator::SetProblemData");
  this->theProblemData.SetKeyValue(problemName, inputData);
}

bool ProblemData::LoadFrom(const std::string& inputData, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("ProblemData::LoadFrom");
  MapLisT<std::string, std::string, MathRoutines::hashString> theMap;
  if (!HtmlRoutines::ChopCGIString(inputData, theMap, commentsOnFailure))
    return false;
  //stOutput << "<hr>DEBUG: Interpreting: <br>" << HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml( inputData )<< "<hr>";
  this->Points = 0;
  this->numCorrectlyAnswered = 0;
  this->totalNumSubmissions = 0;
  this->flagRandomSeedGiven = false;
  if (theGlobalVariables.UserRequestRequiresLoadingRealExamData())
  { if (theMap.Contains("randomSeed"))
    { this->randomSeed = atoi(theMap.GetValueCreate("randomSeed").c_str());
      this->flagRandomSeedGiven = true;
      //stOutput << "<br>DEBUG: random seed found. <br>";
    } //else
      //stOutput << "<br>DEBUG: random seed  NOT NOT NOT found. <br>";
  }
  this->theAnswers.Clear();
  bool result = true;
  MapLisT<std::string, std::string, MathRoutines::hashString> currentQuestionMap;
  for (int i = 0; i < theMap.size(); i ++)
  { if (theMap.theKeys[i] == "randomSeed")
      continue;
    this->AddEmptyAnswerIdOnTop(HtmlRoutines::ConvertURLStringToNormal(theMap.theKeys[i], false));
    Answer& currentA = *this->theAnswers.theValues.LastObject();
    std::string currentQuestion = HtmlRoutines::ConvertURLStringToNormal(theMap.theValues[i], false);
    result=HtmlRoutines::ChopCGIString(currentQuestion, currentQuestionMap, commentsOnFailure);
    if (!result)
    { commentsOnFailure << "Failed to interpret as key-value pair: "
      << currentQuestion << ". ";
      continue;
    }
    if (currentQuestionMap.Contains("numCorrectSubmissions"))
      currentA.numCorrectSubmissions =
      atoi(currentQuestionMap.GetValueCreate("numCorrectSubmissions").c_str());
    if (currentQuestionMap.Contains("numSubmissions"))
      currentA.numSubmissions =
      atoi(currentQuestionMap.GetValueCreate("numSubmissions").c_str());
    if (currentQuestionMap.Contains("firstCorrectAnswer"))
    { currentA.firstCorrectAnswerURLed = currentQuestionMap.GetValueCreate("firstCorrectAnswer");
      currentA.firstCorrectAnswerClean = HtmlRoutines::ConvertURLStringToNormal(currentA.firstCorrectAnswerURLed, false);
      currentA.firstCorrectAnswerURLed = HtmlRoutines::ConvertStringToURLString(currentA.firstCorrectAnswerClean, false); //url-encoding back the cleaned up answer:
      //this protects from the possibility that currentA.firstCorrectAnswerURLed was not encoded properly.
    }
  }
//  this->CheckConsistency();
  return result;
}

std::string ProblemData::Store()
{ MacroRegisterFunctionWithName("ProblemData::Store");
  std::stringstream out;
  if (this->flagRandomSeedGiven)
    out << "randomSeed=" << this->randomSeed;
  for (int i = 0; i < this->theAnswers.size(); i ++)
  { Answer& currentA = this->theAnswers[i];
    if (this->flagRandomSeedGiven || i != 0)
      out << "&";
    out << HtmlRoutines::ConvertStringToURLString(currentA.answerId, false) << "=";
    std::stringstream questionsStream;
    questionsStream
    << "numCorrectSubmissions=" << currentA.numCorrectSubmissions
    << "&numSubmissions=" << currentA.numSubmissions
    << "&firstCorrectAnswer=" << HtmlRoutines::ConvertStringToURLString(currentA.firstCorrectAnswerClean, false);
    out << HtmlRoutines::ConvertStringToURLString(questionsStream.str(), false);
  }
  return out.str();
}

bool UserCalculator::InterpretDatabaseProblemData(const std::string& theInfo, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::InterpretDatabaseProblemData");
  MapLisT<std::string, std::string, MathRoutines::hashString> theMap;
  if (!HtmlRoutines::ChopCGIString(theInfo, theMap, commentsOnFailure))
    return false;
  this->theProblemData.Clear();
  this->theProblemData.SetExpectedSize(theMap.size());
  bool result = true;
  //stOutput << "<hr>DEBUG: Interpreting: <br>" << HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(theInfo)
  //<< "<br>Map has: "
  //<< theMap.size() << " entries. ";
  ProblemData reader;
  std::string probNameNoWhiteSpace;
  for (int i = 0; i < theMap.size(); i ++)
  { //stOutput << "<hr>Reading data: " << theMap.theKeys[i] << ", value: "
    //<< HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(theMap[i]);
    if (!reader.LoadFrom(HtmlRoutines::ConvertURLStringToNormal(theMap[i], false), commentsOnFailure))
    { result = false;
      continue;
    }
    probNameNoWhiteSpace = MathRoutines::StringTrimWhiteSpace(HtmlRoutines::ConvertURLStringToNormal(theMap.theKeys[i], false));
    if (probNameNoWhiteSpace == "")
      continue;
    this->theProblemData.SetKeyValue(probNameNoWhiteSpace, reader);
  }
  return result;
}

List<JSData> UserCalculatorData::GetFindMeFromUserNameQuery()
{ List<JSData> result;
  if (this->username != "")
  { JSData currentJS;
    currentJS[DatabaseStrings::labelUsername] = this->username;
    result.AddOnTop(currentJS);
  }
  if (this->email != "")
  { JSData currentJS;
    currentJS[DatabaseStrings::labelEmail] = this->email;
    result.AddOnTop(currentJS);
  }
  if (result.size == 0)
    crash << "User with find query not allowed to have neither username nor email. " << crash;
  return result;
}

bool UserCalculator::StoreProblemDataToDatabase(std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::StoreProblemDataToDatabase");
  std::stringstream problemDataStream;
  for (int i = 0; i < this->theProblemData.size(); i ++)
    problemDataStream << HtmlRoutines::ConvertStringToURLString(this->theProblemData.theKeys[i], false) << "="
    << HtmlRoutines::ConvertStringToURLString(this->theProblemData.theValues[i].Store(), false) << "&";
//  stOutput << "DEBUG: storing in database ... stack trace: "
//  << crash.GetStackTraceEtcErrorMessage();
  //<< HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(problemDataStream.str());
  JSData setQuery;
  setQuery[DatabaseStrings::labelProblemData] = problemDataStream.str();
  return DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromSomeJSON
  (DatabaseStrings::tableUsers, this->GetFindMeFromUserNameQuery(), setQuery, &commentsOnFailure);
}

bool DatabaseRoutineS::AddUsersFromEmails
(const std::string& emailList, const std::string& userPasswords, std::string& userRole,
 std::string& userGroup, std::stringstream& comments, int& outputNumNewUsers, int& outputNumUpdatedUsers)
{ MacroRegisterFunctionWithName("DatabaseRoutines::AddUsersFromEmails");
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit = 1000;
  theGlobalVariables.MaxComputationTimeBeforeWeTakeAction = 1000;
  List<std::string> theEmails, thePasswords;
  MathRoutines::StringSplitDefaultDelimiters(emailList, theEmails);
  MathRoutines::StringSplitDefaultDelimiters(userPasswords, thePasswords);
  if (thePasswords.size > 0)
    if (thePasswords.size != theEmails.size)
    { comments << "Different number of usernames/emails and passwords: " << theEmails.size << " emails and "
      << thePasswords.size << " passwords. "
      << "If you want to enter usernames without password, you need to leave the password input box empty. ";
      return false;
    }
  //stOutput << " <br>Debug: creating users: " << theEmails.ToStringCommaDelimited()
  //<< "<br>Number of users: " << theEmails.size
  //<< "<br>Number of passwords: " << thePasswords.size << "<hr>Passwords string: " << thePasswords;
  UserCalculator currentUser;
  bool result = true;
  bool doSendEmails = false;
  outputNumNewUsers = 0;
  outputNumUpdatedUsers = 0;
  //stOutput << "DEBUG: courseHome: " << currentUser.currentCourses.value
  //<< "\n<br>\ncurrentUser.currentTable: " << currentUser.currentTable.value << "\n<br>\n";
  for (int i = 0; i < theEmails.size; i ++)
  { currentUser.reset();
    currentUser.username = theEmails[i];
    currentUser.courseInDB = theGlobalVariables.GetWebInput("courseHome");
    currentUser.sectionInDB = userGroup;
    currentUser.instructorInDB = theGlobalVariables.userDefault.username;
    currentUser.email = theEmails[i];
    currentUser.userRole = userRole;

    bool isEmail = true;
    if (theEmails[i].find('@') == std::string::npos)
    { isEmail = false;
      currentUser.email = "";
    } else
      currentUser.email = theEmails[i];
    JSData updateUser;
    List<JSData> findUser;
    findUser.SetSize(2);
    findUser[0][DatabaseStrings::labelUsername] = currentUser.username;
    findUser[1][DatabaseStrings::labelEmail] = currentUser.email;
    updateUser = currentUser.ToJSON();
    if (!DatabaseRoutinesGlobalFunctionsMongo::FindOneFromSome(DatabaseStrings::tableUsers, findUser, updateUser, &comments))
    { if (!currentUser.Iexist(&comments))
      { if (!currentUser.StoreToDB(false, &comments))
        { comments << "Failed to create user: " << currentUser.username;
          result = false;
          continue;
        } else
          outputNumNewUsers ++;
      }
      if (isEmail)
        DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromSomeJSON(DatabaseStrings::tableUsers, findUser, updateUser, &comments);
    } else
      outputNumUpdatedUsers ++;
    //currentUser may have its updated entries modified by the functions above.
    if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromSomeJSON(DatabaseStrings::tableUsers, findUser, updateUser, &comments))
      result = false;
    if (thePasswords.size == 0 || thePasswords.size != theEmails.size)
    { if (currentUser.actualShaonedSaltedPassword == "" && currentUser.actualAuthenticationToken == "")
        if (!currentUser.ComputeAndStoreActivationToken(&comments))
          result = false;
    } else
    { currentUser.enteredPassword = HtmlRoutines::ConvertStringToURLString(thePasswords[i], false);
      //<-Passwords are ONE-LAYER url-encoded
      //<-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
      //<-Incoming pluses must be re-coded as spaces (%20).
      //stOutput << "Debug: user:<br>" << currentUser.username.value << "<br>password:<br>"
      //<< HtmlRoutines::ConvertStringToURLString(thePasswords[i]) << "<br>";
      if (!currentUser.SetPassword(&comments))
        result = false;
      updateUser[DatabaseStrings::labelActivationToken] = "activated";
      DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromSomeJSON(DatabaseStrings::tableUsers, findUser, updateUser, &comments);
      if (currentUser.email != "")
        currentUser.ComputeAndStoreActivationStats(&comments, &comments);
    }
  }
  if (!result)
    comments << "<br>Failed to create all users. ";
  if (doSendEmails)
  { std::stringstream* commentsGeneralSensitive = 0;
    if (theGlobalVariables.UserDefaultHasAdminRights())
      commentsGeneralSensitive = &comments;
    if (!DatabaseRoutineS::SendActivationEmail(theEmails, &comments, &comments, commentsGeneralSensitive))
      result = false;
  }
  return result;
}

bool UserCalculator::GetActivationAbsoluteAddress(std::string& output, std::stringstream& comments)
{ MacroRegisterFunctionWithName("UserCalculator::GetActivationAbsoluteAddress");
  return this->GetActivationAddress
  (output, theGlobalVariables.hopefullyPermanent_HTTPS_WebAdressOfServerExecutable, comments);
}

bool UserCalculator::GetActivationAddress
(std::string& output, const std::string& calculatorBase, std::stringstream& comments)
{ MacroRegisterFunctionWithName("UserCalculator::GetActivationAbsoluteAddress");
  if (!this->LoadFromDB(&comments, &comments))
    return false;
  this->actualActivationToken = this->GetSelectedRowEntry("activationToken");
  if (this->actualActivationToken == "")
  { comments << "Failed to fetch activation token for user: " << this->username;
    return false;
  }
  if (this->actualActivationToken == "activated")
  { comments << "Account of user: " << this->username << "already activated";
    return false;
  }
  output = this->GetActivationAddressFromActivationToken
  (this->actualActivationToken, calculatorBase, this->username, this->email);
  return true;
}

bool UserCalculator::SendActivationEmail(std::stringstream& comments)
{ MacroRegisterFunctionWithName("UserCalculator::SendActivationEmail");
  std::string activationAddress;
  if (!this->GetActivationAbsoluteAddress(activationAddress, comments))
    return false;
//  stOutput << "<hr> all result strings: " << this->selectedRowFieldNamesUnsafe.ToStringCommaDelimited();
//  stOutput << "<br> all result string names: " << this->selectedRowFieldsUnsafe.ToStringCommaDelimited();
  this->email = this->GetSelectedRowEntry("email");
  //theGlobalVariables.FallAsleep(1000000);

  if (this->email == "")
  { comments << "\nNo email address for user: " << this->username << ". ";
    return false;
  }
  EmailRoutines theEmailRoutines;
  theEmailRoutines.toEmail = this->email;
  theEmailRoutines.subject = "NO REPLY: Activation of a Math homework account. ";
  theEmailRoutines.emailContent = "Activation link: " + activationAddress;
//  std::stringstream emailStream;
//  emailStream << "Dear student,\nthis is an automated email sent with an activation token for your "
//  << " math homework account. To activate your account and set up your password, please follow the link below. "
//  << " If you have any technical problems or have questions, please DO NOT HIT the REPLY BUTTON; "
//  << " instead, post your quesiton on piazza.com or email your question to "
//  << " todor.milev@gmail.com\n\n";
//  emailStream << this->activationTokenUnsafe << "\n\nGood luck with our course, \n Your calculus instructors.";
  std::string emailLog = theGlobalVariables.CallSystemWithOutput(theEmailRoutines.GetCommandToSendEmailWithMailX());
  size_t indexGoAhead = emailLog.find("Go ahead");
  bool result = true;
  if (indexGoAhead == std::string::npos)
    result = false;
  else
  { std::string choppedEmailLog=emailLog.substr(indexGoAhead + 8);
    result = (choppedEmailLog.find("OK") != std::string::npos);
  }
  //stOutput << "Calling system with:<br>" << theEmailRoutines.GetCommandToSendEmailWithMailX()
  //<< "<br>\n to get output: \n<br>" << emailLog;
  if (!result)
    comments << "\nFailed to send email to " << this->username << ". Details: \n" << emailLog;
  return result;
}

#include "vpfHeader7DatabaseInterface_Mongodb.h"
#include "vpfHeader3Calculator5_Database_Mongo.h"

bool CalculatorDatabaseFunctions::innerRepairDatabaseEmailRecords
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerRepairDatabaseEmailRecords");
  std::stringstream out;
  if (!theGlobalVariables.UserDefaultHasAdminRights())
  { out << "Function available to logged-in admins only. ";
    return output.AssignValue(out.str(), theCommands);
  }
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit = 20000;
  (void) input;//prevent unused parameter, portable
  out << "Testing/repairing database ... Comments:<br>";
  out << "NOT IMPLEMENTED YET";
  return output.AssignValue(out.str(), theCommands);
/*

  std::stringstream comments;
  theRoutines.startMySQLDatabase(&comments, 0);
  if (!theRoutines.ColumnExists(DatabaseStrings::labelCourseInfo, DatabaseStrings::tableUsers, out))
  { out << "Column " << DatabaseStrings::labelCourseInfo << ": "
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
  if (emailColumn == - 1)
  { out << "Couldn't find email column. ";
    return output.AssignValue(out.str(), theCommands);
  }
  if (usernameColumn == - 1)
  { out << "Couldn't find username column. ";
    return output.AssignValue(out.str(), theCommands);
  }
  if (passwordColumn == - 1)
  { out << "Couldn't find password column. ";
    return output.AssignValue(out.str(), theCommands);
  }
  if (activationTokenColumn == - 1)
  { out << "Couldn't find activation token column. ";
    return output.AssignValue(out.str(), theCommands);
  }
  HashedList<std::string, MathRoutines::hashString> emailsRegistered;
  for (int i = 0; i < theUserTable.size; i++)
  { std::string currentEmail = theUserTable[i][emailColumn];
    if (emailsRegistered.Contains(currentEmail))
    { out << "Fatal error: email " << currentEmail << "repeated. ";
      return output.AssignValue(out.str(), theCommands);
    }
  }
  int numCorrections = 0;
  UserCalculator currentUser;
  currentUser.currentTable = DatabaseStrings::tableUsers;
  for (int i = 0; i < theUserTable.size; i ++)
  { std::string currentUserName = theUserTable[i][usernameColumn];
    std::string currentEmail = theUserTable[i][emailColumn];
    if (currentEmail != "")
      continue;
    if (!EmailRoutines::IsOKEmail(currentUserName, 0))
      continue;
    if (emailsRegistered.Contains(currentEmail))
    { out << "<br>This shouldn't happen: username: " << currentUserName
      << " has no corresponding email. At the same time, the username appears to be an email, and "
      << " that same email has been registered with another account. "
      << " Please resolve the matter with the user. ";
      continue;
    }
    currentUser.username = currentUserName;
    currentUser.email = currentUserName;
    if (!currentUser.SetColumnEntry("email", currentUser.email.value, theRoutines, &comments))
    { out << "<br>This shouldn't happen: failed to set email for " << currentUser.username.value << ". ";
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
  for (int i = 0; i < theUserTable.size; i ++)
  { currentUser.username = theUserTable[i][usernameColumn];
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
  for (int i = 0; i < theUserTable.size; i ++)
  { currentUser.reset();
    std::stringstream currentUserComments;
    currentUser.username = theUserTable[i][usernameColumn];
    currentUser.FetchOneUserRow(theRoutines, &out, &currentUserComments);
    if (currentUserComments.str().size() > 0)
    { numCorrections ++;
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

bool EmailRoutines::SendEmailWithMailGun
(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral,
 bool includeEmailCommandInComments)
{ MacroRegisterFunctionWithName("EmailRoutines::SendEmailWithMailGun");
  std::string mailGunKey, hostnameToSendEmailFrom;
  if (!FileOperations::LoadFileToStringVirtual("certificates/mailgun-api.txt", mailGunKey, true, true, commentsOnFailure))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Could not find mailgun key. The key must be located in file: "
      << "<br>\ncertificates/mailgun-api.txt\n<br>\n "
      << "The file must be uploaded manually to the server. ";
    return false;
  }
  if (!FileOperations::LoadFileToStringVirtual("certificates/mailgun-hostname.txt", hostnameToSendEmailFrom, true, true, 0))
  { hostnameToSendEmailFrom = theGlobalVariables.hostNoPort;
    if (theGlobalVariables.UserDefaultHasAdminRights() && commentsGeneral != 0)
    { *commentsGeneral << "Did not find the mailgun hostname file: certificates/mailgun-hostname.txt. Using the "
      << "domain name: " << hostnameToSendEmailFrom << " instead. ";
    }
  } else
  { hostnameToSendEmailFrom = MathRoutines::StringTrimWhiteSpace(hostnameToSendEmailFrom);
    if (theGlobalVariables.UserDefaultHasAdminRights() && commentsGeneral != 0)
    { *commentsGeneral << "Hostname loaded: "
      << HtmlRoutines::ConvertStringToURLString(hostnameToSendEmailFrom, false) << " instead. ";
    }
  }

  if (mailGunKey.size() > 0)
    mailGunKey.resize(mailGunKey.size() - 1);
  std::stringstream commandToExecute;
  commandToExecute << "curl -s --user 'api:" << mailGunKey
  << "' ";
  commandToExecute
  << "https://api.mailgun.net/v3/mail2."
  << hostnameToSendEmailFrom
  << "/messages "
  ;
  commandToExecute << "-F from='Automated Email "
  << "<noreply@mail2."
  << hostnameToSendEmailFrom
  << ">' ";
  if (this->toEmail == "")
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Receiver address is missing. ";
    return false;
  }
  if (this->subject == "" && this->emailContent == "")
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Empty emails not allowed. ";
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
  << "\""
  ;
  std::string commandResult = theGlobalVariables.CallSystemWithOutput(commandToExecute.str());
  if (commentsGeneral != 0)
  { if (includeEmailCommandInComments)
      *commentsGeneral << "Command: " << HtmlRoutines::ConvertStringToHtmlString(commandToExecute.str(), true);
    bool isBad = false;
    if (commandResult.find("Forbidden") != std::string::npos)
      isBad = true;
    *commentsGeneral << "<br>Result:<br>";
    if (isBad)
      *commentsGeneral << "<span style=\"color:red\"><b>";
    *commentsGeneral << HtmlRoutines::ConvertStringToHtmlString(commandResult, true);
    if (isBad)
      *commentsGeneral << "</b></span>";
  }
  return true;
}

List<bool> EmailRoutines::recognizedEmailCharacters;
List<bool>& EmailRoutines::GetRecognizedEmailChars()
{ if (recognizedEmailCharacters.size == 0)
  { recognizedEmailCharacters.initFillInObject(256, false);
    std::string theChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    theChars += "0123456789";
    theChars += "@";
    theChars += "!#$%&'*+-/=?.";
    for (unsigned i = 0; i < theChars.size(); i ++)
      recognizedEmailCharacters[((unsigned char)theChars[i])]=true;
  }
  return EmailRoutines::recognizedEmailCharacters;
}

bool EmailRoutines::IsOKEmail(const std::string& input, std::stringstream* commentsOnError)
{ MacroRegisterFunctionWithName("EmailRoutines::IsOKEmail");
  //stOutput << "DEBUG: got to isokemail";
  if (input.size() == 0)
  { if (commentsOnError != 0)
      *commentsOnError << "Empty email not allowed. ";
    return false;
  }
  int numAts = 0;
  for (unsigned i = 0; i < input.size(); i ++)
  { if(!EmailRoutines::GetRecognizedEmailChars()[((unsigned char) input[i])])
    { if (commentsOnError != 0)
        *commentsOnError << "Email: " << input << " contains the unrecognized character "
        << input[i] << ". ";
      return false;
    }
    if (input[i] == '@')
      numAts ++;
  }
  if (numAts != 1 && commentsOnError != 0)
    *commentsOnError << "Email: "
    << input << " is required to have exactly one @ character. ";
  return numAts == 1;
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
{ this->fromEmail = "calculator.todor.milev@gmail.com";
  //this->ccEmail="todor.milev@gmail.com";
  this->smtpWithPort = "smtp.gmail.com:587";
  this->fromEmailAuth = Crypto::ConvertStringToBase64("A good day to use a computer algebra system");
}

#endif //MACRO_use_MySQL

bool DatabaseRoutinesGlobalFunctions::LoginViaGoogleTokenCreateNewAccountIfNeeded
(UserCalculatorData& theUseR, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral)
{ (void) commentsOnFailure;
  (void) theUseR;
  (void) commentsGeneral;
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::LoginViaGoogleTokenCreateNewAccountIfNeeded");
  UserCalculator userWrapper;
  userWrapper.::UserCalculatorData::operator=(theUseR);
  if (userWrapper.enteredGoogleToken == "")
    return false;
  if (!Crypto::VerifyJWTagainstKnownKeys(userWrapper.enteredGoogleToken, commentsOnFailure, commentsGeneral))
    return false;
  JSONWebToken theToken;
  if (!theToken.AssignString(userWrapper.enteredGoogleToken, commentsOnFailure))
    return false;
  JSData theData;
  if (!theData.readstring(theToken.claimsJSON, false, commentsOnFailure))
    return false;
  if (theData.GetValue("email").type != JSData::JSstring)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Could not find email entry in the json data " << theData.ToString(true);
    return false;
  }
  userWrapper.email = theData.GetValue("email").string;
  userWrapper.username = "";
  if (!userWrapper.Iexist(commentsOnFailure))
  { if (commentsGeneral != 0)
      *commentsGeneral << "User with email " << userWrapper.email << " does not exist. ";
    //stOutput << "\n<br>\nDEBUG: User with email " << userWrapper.email.value << " does not exist. ";
    userWrapper.username = userWrapper.email;
    if (!userWrapper.StoreToDB(false, commentsOnFailure))
      return false;
    if (commentsGeneral != 0)
      *commentsGeneral << "Created new user with username: " << userWrapper.username;
    theUseR = userWrapper;
    return true;
  }
  if (!userWrapper.LoadFromDB(commentsOnFailure))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Failed to fetch user with email " << userWrapper.email << ". ";
    return false;
  }
  if (userWrapper.actualAuthenticationToken == "")
    userWrapper.ResetAuthenticationToken(commentsOnFailure);
  theUseR = userWrapper;
  return true;
#else
  return true;
#endif
}

bool DatabaseRoutinesGlobalFunctions::LoginViaDatabase(UserCalculatorData& theUseR, std::stringstream* comments)
{ (void) comments;
  (void) theUseR;
#ifdef MACRO_use_MySQL
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctions::LoginViaDatabase");
  UserCalculator userWrapper;
  userWrapper.::UserCalculatorData::operator=(theUseR);
  if (userWrapper.Authenticate(comments))
  { theUseR = userWrapper;
    return true;
  }
  //*comments << "DEBUG: entered pass: " << userWrapper.enteredPassword
  //<< "shaone-ing to: " << userWrapper.enteredShaonedSaltedPassword
  //<< ". Actual pass sha: " << userWrapper.actualShaonedSaltedPassword;
  if (userWrapper.enteredAuthenticationToken != "" &&
      userWrapper.enteredActivationToken == "" &&
      userWrapper.enteredAuthenticationToken != "0" &&
      comments != 0)
  { *comments << "<b> Authentication of user: " << userWrapper.username
    << " with token " << userWrapper.enteredAuthenticationToken << " failed. </b>";
  }
  if (userWrapper.enteredActivationToken != "" && comments != 0)
  { //*comments << "DEBUG: actual activation token: " << userWrapper.actualActivationToken
    //<< ". Entered activation token: " << userWrapper.enteredActivationToken
    //<< "user request: " << theGlobalVariables.userCalculatorRequestType;
  }
  if (theGlobalVariables.userCalculatorRequestType == "changePassword" ||
      theGlobalVariables.userCalculatorRequestType == "changePasswordPage" ||
      theGlobalVariables.userCalculatorRequestType == "activateAccount" )
    if (userWrapper.enteredActivationToken != "")
    { if (userWrapper.actualActivationToken != "activated" &&
          userWrapper.actualActivationToken != "" &&
          userWrapper.actualActivationToken != "error")
      { if (userWrapper.enteredActivationToken == userWrapper.actualActivationToken)
        { theUseR = userWrapper;
          return true;
        }
      } else if (comments != 0)
      { if (userWrapper.actualActivationToken != "error")
          *comments << "<b>Account already activated. </b>";
        else
          *comments << "<b>An error during activation ocurred.</b>";
      }
    }
  if (userWrapper.username == "admin" && userWrapper.enteredPassword != "")
    if (!userWrapper.Iexist(0))
    { if (comments != 0)
        *comments << "First login of user admin: setting admin pass.";
      //*comments << "DEBUG: user before storing: " << userWrapper.ToJSON().ToString();
      userWrapper.actualActivationToken = "activated";
      userWrapper.userRole = "admin";
      userWrapper.StoreToDB(true, comments);
      //*comments << "DEBUG: admin user: " << userWrapper.ToJSON().ToString() ;
      theUseR = userWrapper;
      return true;
    }
  return false;
#else
  return true;
#endif
}

//meld comment
#ifdef MACRO_use_MySQL

#include "../../calculator/src/vpfHeader5Crypto.h"
//#include <time.h>
//#include <ctime>

bool UserCalculator::StoreToDB(bool doSetPassword, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("UserCalculator::StoreToDB");
  JSData findUser;
  findUser[DatabaseStrings::labelUsername] = this->username;
  if (this->enteredPassword != "" && doSetPassword)
  { //*commentsOnFailure << " DEBUG: Computing sha-1 salted pass from: " << this->enteredPassword;
    this->ComputeShaonedSaltedPassword();
    this->actualShaonedSaltedPassword = this->enteredShaonedSaltedPassword;
  }
  JSData setUser = this->ToJSON();
  //*commentsOnFailure << " DEBUG: About to Store: " << setUser.ToString();

  return DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(DatabaseStrings::tableUsers, findUser, setUser, commentsOnFailure);
}

std::string UserCalculator::GetActivationAddressFromActivationToken
(const std::string& theActivationToken, const std::string& calculatorBase,
 const std::string& inputUserNameUnsafe, const std::string& inputEmailUnsafe)
{ MacroRegisterFunctionWithName("UserCalculator::GetActivationLinkFromActivationToken");
  std::stringstream out;
  if (MathRoutines::StringBeginsWith(calculatorBase, "localhost"))
    out << "https://" << calculatorBase;
  else if(MathRoutines::StringBeginsWith(calculatorBase, "https://localhost"))
    out << calculatorBase;
  else
    out << theGlobalVariables.hopefullyPermanentWebAdress;
  out << theGlobalVariables.DisplayNameExecutable
  << "?request=activateAccount&username="
  << HtmlRoutines::ConvertStringToURLString(inputUserNameUnsafe, false);
  if (inputEmailUnsafe != "")
    out << "&email=" << HtmlRoutines::ConvertStringToURLString(inputEmailUnsafe, false);
  out << "&activationToken=" << HtmlRoutines::ConvertStringToURLString(theActivationToken, false);
  return out.str();
}

#endif
